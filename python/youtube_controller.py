import pyautogui
import serial
import argparse
import time
import logging

from pycaw.pycaw import AudioUtilities, IAudioEndpointVolume
from ctypes import cast, POINTER
from comtypes import CLSCTX_ALL

devices = AudioUtilities.GetSpeakers()
interface = devices.Activate(IAudioEndpointVolume._iid_, CLSCTX_ALL, None)
volume = cast(interface, POINTER(IAudioEndpointVolume))

# # Get current volume 
# currentVolumeDb = volume.GetMasterVolumeLevel()
# volume.SetMasterVolumeLevel(currentVolumeDb - 6.0, None)

class MyControllerMap:
    def __init__(self):
        self.button = {'AZUL': 'up',
                       'VERMELHO': 'down',
                       'VERDE': 'space'} 

class SerialControllerInterface:
    # Protocolo
    # byte 1 -> Botão 1 (estado - Apertado 1 ou não 0)
    # byte 2 -> EOP - End of Packet -> valor reservado 'X'

    def __init__(self, port, baudrate):
        self.ser = serial.Serial(port, baudrate=baudrate)
        self.mapping = MyControllerMap()
        self.incoming = '0'
        self.handshake = False
        self.current_volume = volume.GetMasterVolumeLevel()
        pyautogui.PAUSE = 0  ## remove delay
    
    def handshake_protocol(self):
        while True:
            self.ser.write(b'1')

            data = self.ser.read()
            logging.debug("Received DATA: {}".format(data))
            if data == b'1':
                logging.info("HANDSHAKE")
                self.incoming = self.ser.read()
                break
            
        self.handshake = True

    def update(self):
        ## Sync protocol
        # while self.incoming != b'X':
            
        #     self.incoming = self.ser.read()
        #     logging.debug("Received INCOMING: {}".format(self.incoming))

        # ----------   RECEBENDO TAMANHO DO PACOTE   ----------
        len_data = self.ser.read()
        if len_data == b'1':
            # ----------   RECEBENDO TAMANHO   ----------
            data = self.ser.read()
            # logging.debug("Received DATA: {}".format(data))

            if data == b'1':
                logging.info("BOTÃO AZUL APERTADO")
                pyautogui.keyDown(self.mapping.button['AZUL'])
                pyautogui.keyUp(self.mapping.button['AZUL'])
        
            elif data == b'2':
                logging.info("BOTÃO VERMELHO APERTADO")
                pyautogui.keyDown(self.mapping.button['VERMELHO'])

            elif data == b'3':
                logging.info("BOTÃO VERMELHO SOLTO")
                pyautogui.keyUp(self.mapping.button['VERMELHO'])

            elif data == b'4':
                logging.info("BOTÃO VERDE APERTADO")
                pyautogui.keyDown(self.mapping.button['VERDE'])
                pyautogui.keyUp(self.mapping.button['VERDE'])

            elif data == b'5':
                logging.info("BOTÃO PRETO APERTADO")
                # pyautogui.keyDown(self.mapping.button['VERDE'])
                # pyautogui.keyUp(self.mapping.button['VERDE'])

            # # ----------   RECEBENDO EOF   ----------
            # self.incoming = self.ser.read()

        elif len_data == b'2':
            data_1 = self.ser.read()
            data_2 = self.ser.read()

            data_1 = int.from_bytes(data_1, "little")
            data_2 = int.from_bytes(data_2, "little")
            vol = (data_2 << 8) | data_1
            vol = 51*(vol - 22500)/(22850 - 22500) - 51
            logging.info(f"VOLUME = {vol}")
            volume.SetMasterVolumeLevel(vol, None)
            # # ----------   RECEBENDO EOF   ----------
            # self.incoming = self.ser.read()


class DummyControllerInterface:
    def __init__(self):
        self.mapping = MyControllerMap()

    def update(self):
        pyautogui.keyDown(self.mapping.button['A'])
        # time.sleep(0.1)
        pyautogui.keyUp(self.mapping.button['A'])
        logging.info("[Dummy] Pressed A button")
        # time.sleep(1)


if __name__ == '__main__':
    interfaces = ['dummy', 'serial']
    argparse = argparse.ArgumentParser()
    argparse.add_argument('serial_port', type=str)
    argparse.add_argument('-b', '--baudrate', type=int, default=9600)
    argparse.add_argument('-c', '--controller_interface', type=str, default='serial', choices=interfaces)
    argparse.add_argument('-d', '--debug', default=False, action='store_true')
    args = argparse.parse_args()
    if args.debug:
        logging.basicConfig(level=logging.DEBUG)

    print("Connection to {} using {} interface ({})".format(args.serial_port, args.controller_interface, args.baudrate))
    if args.controller_interface == 'dummy':
        controller = DummyControllerInterface()
    else:
        controller = SerialControllerInterface(port=args.serial_port, baudrate=args.baudrate)

    controller.handshake_protocol()
    while True and controller.handshake:
        controller.update()
