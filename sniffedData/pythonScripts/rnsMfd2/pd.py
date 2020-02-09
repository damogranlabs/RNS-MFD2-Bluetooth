##
# This file is part of the libsigrokdecode project.
##
# Copyright (C) 2010-2016 Uwe Hermann <uwe@hermann-uwe.de>
##
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
##
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
##
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <http://www.gnu.org/licenses/>.
##
import enum

import sigrokdecode as srd

PULSE_TOLERANCE_TIME_US = 10

DATA_PIN_INDEX = 0

ANNOTATION_BIT_INDEX = 0
ANNOTATION_START_COND_INDEX = 1
ANNOTATION_DATA_INDEX = 2
ANNOTATION_STOP_INDEX = 3


class States(enum.Enum):
    # [annotationTypeIndex, longAnnotation, shortAnnotation]
    IDLE = ['idle', 'I']
    START = ['START', 'S']
    DATA = ['DATA', 'D']
    STOP = ['STOP', 'X']


class Decoder(srd.Decoder):
    api_version = 3
    id = 'rnsMfd2'
    name = 'RNS_MFD2'
    longname = 'RNS MFD2 data out'
    desc = 'VW RND MFD2 -> CD Changer data protocol.'
    license = 'gplv2+'
    inputs = ['logic']
    outputs = ['radioDataout']
    channels = (
        {'id': 'dataOut', 'name': 'DATA_OUT', 'desc': 'Radio data out line'},
    )

    annotations = (                     # Implicitly assigned annotation type ID
        ('bit', 'Bit'),                 # 0
        ('start', 'Start condition'),   # 1
        ('data-out', 'Data out'),       # 2
        ('stop', 'Stop condition'),     # 3
    )
    annotation_rows = (
        ('bits', 'Bits', (0, 1, 3)),
        ('fields', 'Fields', (2, )),
    )
    def __init__(self, **kwargs):
        self.sampleRateHz = None
        self.toleranceInSamples = 0

        self.state = States.IDLE

        self.bits = []
        self.byteStartSampleIndex = 0
    
    def metadata(self, key, value):
        # Currently the only value for key is sigrokdecode.SRD_CONF_SAMPLERATE, value is then the sample rate of the data stream in Hz.
        if key == srd.SRD_CONF_SAMPLERATE:
            self.sampleRateHz = value
            durationOfOneSample = 1 / self.sampleRateHz  # value should be in Hz
            toleranceTimeSec = PULSE_TOLERANCE_TIME_US * 1e-6
            self.toleranceInSamples = int(toleranceTimeSec / durationOfOneSample)

    def start(self):
        """
        This function is called before the beginning of the decoding. 
        This is the place to register() the output types, check the user-supplied PD options for validity, and so on.
        """
        self.out_ann = self.register(srd.OUTPUT_ANN)
        # self.register() This function is used to register the output that will be generated by the decoderThe function returns an identifier that can then be used as the output_id argument of the put() function.

    def _putBit(self, startSample, endSample, bitState):
        if bitState:
            bitState = "1"
        else:
            bitState = "0"
        self.put(startSample, endSample, self.out_ann, [ANNOTATION_BIT_INDEX, [str(bitState)]])

    def _putField(self, startSample, endSample, fieldValue):
        self.put(startSample, endSample, self.out_ann, [ANNOTATION_BIT_INDEX, fieldValue])

    def _putData(self, startSample, endSample, fieldValue):
        self.put(startSample, endSample, self.out_ann, [ANNOTATION_DATA_INDEX, fieldValue])

    def decode(self, *args):
        """
        in non-stacked decoders, this function is called by the libsigrokdecode backend to start the decoding.
        It takes no arguments, but instead will enter an infinite loop and gets samples by calling the more versatile wait() method. 
        """
        dataStartSampleIndex = None
        self.state = States.IDLE

        while True:
            if self.state == States.IDLE:  # IDLE state, find start
                dataStartSampleIndex = self._waitForStartCondition(dataStartSampleIndex)
            
            elif self.state == States.DATA:
                dataStartSampleIndex = self._waitForPinChange(dataStartSampleIndex)

    def _waitForStartCondition(self, startCondSampleIndex):
        """
        Wait for start condition HIGH - LOW pulse.
        """
        if startCondSampleIndex is None:
            self.wait({DATA_PIN_INDEX: 'r'})
            startCondSampleIndex = self.samplenum

        self.wait({DATA_PIN_INDEX: 'f'})
        self.wait({DATA_PIN_INDEX: 'r'})
        startCondEndSampleIndex = self.samplenum

        self._putField(startCondSampleIndex, startCondEndSampleIndex, States.START.value)

        self.state = States.DATA

        self.bits.clear()

        return startCondEndSampleIndex

    def _waitForPinChange(self, dataStartSampleIndex):
        """
        Handle line toggling in DATA state
        """
        self.wait({DATA_PIN_INDEX: 'f'})
        intermediatesampleIndex = self.samplenum
        self.wait({DATA_PIN_INDEX: 'r'})

        self.state = States.DATA
        if self._isLogicOne(dataStartSampleIndex, intermediatesampleIndex, self.samplenum):
            if self._isEndOfData(dataStartSampleIndex, intermediatesampleIndex, self.samplenum):
                self.bits.clear()
                highPulseLen = (intermediatesampleIndex - dataStartSampleIndex)
                stopBitSampleNum = intermediatesampleIndex + highPulseLen
                self._putField(dataStartSampleIndex, stopBitSampleNum, States.STOP.value)

                self.state = States.IDLE
            else:
                self._putBit(dataStartSampleIndex, self.samplenum, True)
                self.bits.append('1')
        else:
            self._putBit(dataStartSampleIndex, self.samplenum, False)
            self.bits.append('0')

        if len(self.bits) == 1:
            self.byteStartSampleIndex = dataStartSampleIndex
        elif len(self.bits) == 8:
            bitsStr = ''.join(reversed(self.bits)) # LSB first?
            value = "0x%X" % int(bitsStr, 2)
            self._putData(self.byteStartSampleIndex, self.samplenum, [value])
            self.bits.clear()

        return self.samplenum

    def _isLogicOne(self, dataStartSampleIndex, intermediatesampleIndex, nextDataStartSampleIndex):
        # if pulse (low vs high) are somewhat similar length, this is logic 1, else 0
        highPulseLen = intermediatesampleIndex - dataStartSampleIndex
        lowPulseLen = nextDataStartSampleIndex - intermediatesampleIndex

        pulseDiffLen = abs(highPulseLen - lowPulseLen)
        if pulseDiffLen > (self.toleranceInSamples):
            return True
        else:
            return False

    def _isEndOfData(self, dataStartSampleIndex, intermediatesampleIndex, nextDataStartSampleIndex):
        # if low pulse is very long, this might indicate end of data state
        # checked if low pulse is at least two size of high pulse
        highPulseLen = intermediatesampleIndex - dataStartSampleIndex
        lowPulseLen = nextDataStartSampleIndex - intermediatesampleIndex

        if lowPulseLen > (highPulseLen * 4):
            return True
        else:
            return False
