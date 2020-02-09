"""
Group SPI data bytes into 8-byte packets.

sigrok-cli.exe -i "D:\Stromarija\projects\cdcEmulator\cdc_init2.sr" -P spi:clk="D2 - CLK":mosi="D0 - Data OUT":cpha=1 -P rnsMfd2:dataOut="D1 - Data IN" -A spi=mosi-data,rnsMfd2=data-out --protocol-decoder-samplenum >> "D:\Stromarija\projects\cdcEmulator\dataExports\init.txt"
"""
import sys
import os
import subprocess
import shutil

SIGROK_PATH = "C:\\Program Files (x86)\\sigrok\\sigrok-cli\\sigrok-cli.exe"
CDC_OUTPUT_PIN = "DOUT"  # "D0_CDC_OUT"
CDC_INPUT_PIN = "DIN"  # "D1_CDC_IN"
CDC_OUT_CLK = "CLK"  # "D2_CDC_OUT_CLK"


CDC_DATA_OUT_TAG = "spi-1"
CDC_DATA_OUT_BLOCK_NUM_BYTES = 8
CDC_DATA_IN_TAG = "rnsMfd2-1"
CDC_DATA_IN_BLOCK_NUM_BYTES = 4

SAMPLING_FREQ_KHZ = 500
_sampleTime = 1 / (SAMPLING_FREQ_KHZ * 1e3)

ANALYZER_FILES_ROOT_FOLDER = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'sniffedData')
ANALYZER_EXPORT_FOLDER = os.path.join(ANALYZER_FILES_ROOT_FOLDER, 'dataExports')
ANALYZER_RAW_EXPORT_FOLDER = os.path.join(ANALYZER_EXPORT_FOLDER, 'raw')


class DataDirection:
    IN = 'IN'
    OUT = 'OUT'


class Data:
    def __init__(self, startSample, endSample, data, direction):
        self.startSample = startSample
        self.endSample = endSample
        self.data = data
        self.direction: DataDirection = direction
        self.startTimestamp: int = startSample * _sampleTime
        self.endTimestamp: int = startSample * _sampleTime


def getFileData(filePath) -> (list, list):
    # open file
    with open(filePath, 'r') as fHandler:
        lines = fHandler.readlines()

    cdcDataOut = []
    cdcDataIn = []
    for line in lines:
        line = line.strip()
        if CDC_DATA_OUT_TAG in line:
            # example: 2303985-2304052 spi-1: BC
            sampleStr, _, data = line.split(' ')
            startSample, endSample = sampleStr.split('-')
            data = Data(int(startSample), int(endSample), f"0x{data}", DataDirection.OUT)
            cdcDataOut.append(data)

        if CDC_DATA_IN_TAG in line:
            # example: 923392-930128 rnsMfd2-1: 0xAC
            sampleStr, _, data = line.split(' ')
            startSample, endSample = sampleStr.split('-')
            data = Data(int(startSample), int(endSample), data, DataDirection.IN)
            cdcDataIn.append(data)

    return cdcDataOut, cdcDataIn


def mergeDataToBlocks(dataOut: list, dataIn: list) -> (list, list):
    dataOutBlocks = []
    for dataIndex in range(0, len(dataOut), CDC_DATA_OUT_BLOCK_NUM_BYTES):
        chunk = dataOut[dataIndex: (dataIndex + CDC_DATA_OUT_BLOCK_NUM_BYTES)]
        dataOutBlocks.append(chunk)

    dataInBlocks = []
    for dataIndex in range(0, len(dataIn), CDC_DATA_IN_BLOCK_NUM_BYTES):
        chunk = dataIn[dataIndex: (dataIndex + CDC_DATA_IN_BLOCK_NUM_BYTES)]
        dataInBlocks.append(chunk)

    return dataOutBlocks, dataInBlocks


def correlateInOutData(outBlocks, inBlocks) -> list:
    allBlocks = []
    allBlocks.extend(outBlocks)
    allBlocks.extend(inBlocks)

    allBlocks2 = sorted(allBlocks, key=lambda data: data[-1].endSample)  # startSample)

    return allBlocks2


def smartPrint(blocks, onlyRelevantSamples: bool = True, neighborSampleCount: int = 8):
    printData = []

    if onlyRelevantSamples:
        # get relevant blocks
        # found start & end index
        startSampleIndex = 0
        for blockIndex, block in enumerate(blocks):
            if block[0].direction == DataDirection.IN:
                startSampleIndex = blockIndex - neighborSampleCount
                break
        endSampleIndex = 0
        for blockIndex, block in reversed(list(enumerate(blocks))):
            if block[0].direction == DataDirection.IN:
                endSampleIndex = blockIndex + neighborSampleCount
                break

        if startSampleIndex < 0:
            startSampleIndex = 0
        if endSampleIndex > len(blocks):
            endSampleIndex = len(blocks)
        relevantBlocks = blocks[startSampleIndex: endSampleIndex]

        printData.append('...\n')
        for block in relevantBlocks:
            msg = addIOTags(block)
            printData.append(msg)
        printData.append('...\n')
    else:
        for block in blocks:
            msg = addIOTags(block)
            printData.append(msg)

    return printData


def addIOTags(blockData) -> str:
    thisBlockData = []
    for data in blockData:
        thisBlockData.append(data.data)  # data: Data = data
    mergedData = '; '.join(thisBlockData)

    if data.direction == DataDirection.OUT:
        msg = f"-> {mergedData}"
    else:
        msg = f"    <- {mergedData}"

    msg = msg.ljust(60) + f"{data.endTimestamp} (sec)\n"
    return msg


def analyzeFile(filePath, onlyRelevantSamples: bool = True, neighborSampleCount: int = 8):
    dataOut, dataIn = getFileData(filePath)
    outBlocks, inBlocks = mergeDataToBlocks(dataOut, dataIn)
    blocks = correlateInOutData(outBlocks, inBlocks)

    printData = smartPrint(blocks, onlyRelevantSamples, neighborSampleCount)

    return printData


def exportFiles(srcFolderPath, dstFolderPath):
    if os.path.exists(dstFolderPath):
        shutil.rmtree(dstFolderPath)
    os.mkdir(dstFolderPath)

    relevantFiles = os.listdir(srcFolderPath)
    for theFile in relevantFiles:
        filePath = os.path.join(srcFolderPath, theFile)
        name, ext = os.path.splitext(theFile)
        if ext == '.sr':
            dstFilePath = os.path.join(dstFolderPath, name + '.data')
            sigrokAnalyze(filePath, dstFilePath)


def generateReportFilesFromRawData(srcFolderPath, dstFolderPath):
    relevantFiles = os.listdir(srcFolderPath)
    for theFile in relevantFiles:
        filePath = os.path.join(srcFolderPath, theFile)
        name, ext = os.path.splitext(theFile)
        if ext == '.data':
            dstFilePath = os.path.join(dstFolderPath, name + '.txt')
            lines = generateReportFile(filePath, dstFilePath)
            print(name + " ######################################")
            print(''.join(lines))
            print("\n\n")


def generateReportFile(srcFilePath, dstFilePath, onlyRelevantSamples: bool = True, neighborSampleCount: int = 8) -> str:
    lines = analyzeFile(srcFilePath, onlyRelevantSamples, neighborSampleCount)
    with open(dstFilePath, 'w+') as fHandler:
        fHandler.writelines(lines)

    return lines


def sigrokAnalyze(srcFilePath, dstFilePath):
    args = []
    args.append("\"" + SIGROK_PATH + "\"")
    args.append("-i")
    args.append("\"" + srcFilePath + "\"")

    args.append("-P")
    args.append(f"spi:clk=\"{CDC_OUT_CLK}\":mosi=\"{CDC_OUTPUT_PIN}\":cpha=1")
    args.append("-P")
    args.append(f"rnsMfd2:dataOut=\"{CDC_INPUT_PIN}\"")

    args.append("-A")
    args.append("spi=mosi-data,rnsMfd2=data-out")

    args.append("--protocol-decoder-samplenum")

    args.append(">>")
    args.append("\"" + dstFilePath + "\"")

    command = ' '.join(args)
    try:
        subprocess.run(command, check=True, shell=True)
    except Exception as err:
        print("Sigrok analyze error:", command)
        raise


def generateDataReports():
    thisFile = os.path.abspath(__file__)

    exportFiles(ANALYZER_FILES_ROOT_FOLDER, ANALYZER_RAW_EXPORT_FOLDER)
    generateReportFilesFromRawData(ANALYZER_RAW_EXPORT_FOLDER, ANALYZER_EXPORT_FOLDER)


if len(sys.argv) > 1:
    _, inputFileName = os.path.split(sys.argv[1])
    rawFileName = inputFileName.replace(".sr", ".data")
    reportFileName = inputFileName.replace(".sr", ".txt")

    dstFilePath = os.path.join(ANALYZER_RAW_EXPORT_FOLDER, rawFileName)
    if os.path.exists(dstFilePath):
        os.unlink(dstFilePath)

    dstReportFilePath = os.path.join(ANALYZER_EXPORT_FOLDER, reportFileName)

    sigrokAnalyze(sys.argv[1], dstFilePath)
    lines = generateReportFile(dstFilePath, dstReportFilePath, False)

    for line in lines:
        print(line.strip('\n'))

    os.system("start " + dstReportFilePath)
else:
    generateDataReports()
