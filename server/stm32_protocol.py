
import serial
import time

class STM32Serial:
    def __init__(self, port="/dev/ttyS0", baudrate=115200, timeout=1):
        self.ser = serial.Serial(port, baudrate, timeout=timeout)
        time.sleep(0.1)

    # ----------- Fonction interne pour envoyer une commande ----------
    def _send_cmd(self, cmd):
        """Envoie 'cmd' au STM32, retourne la ligne reçue."""
        message = (cmd).encode()
        self.ser.write(message)
        self.ser.flush()
        return self.ser.readline().decode().strip()

    # ----------- GET_T : température compensée -----------------------
    def get_temperature(self):
        resp = self._send_cmd("GET_T")
        hex_value = resp[2:-1]
        decimal_value = int(hex_value, 16)
        return decimal_value/100  # ex: "T=+12.50_C"

    # ----------- GET_P : pression compensée --------------------------
    def get_pressure(self):
        resp = self._send_cmd("GET_P")
        hex_value = resp[2:-1]
        decimal_value = int(hex_value, 16)
        return decimal_value  # ex: "P=102300Pa"

    # ----------- SET_K : fixer coefficient K -------------------------
    def set_k(self, value):
        """
        value = entier entre 0 et 9999 (K * 100)
        Exemple : value=1234 → SET_K=1234
        """
        self._send_cmd("SET_K")
        resp = self._send_cmd(f"{value}")
        return resp  # ex: "SET_K=OK"

    # ----------- GET_K : lire coefficient K --------------------------
    def get_k(self):
        resp = self._send_cmd("GET_K")
        return resp  # ex: "K=12.34000"

    # ----------- GET_A : angle ---------------------------------------
    def get_angle(self):
        resp = self._send_cmd("GET_A")
        return resp  # ex: "A=125.7000"

    # ----------- Fermeture UART --------------------------------------
    def close(self):
        self.ser.close()


# =====================================================================
# Exemple d'utilisation
# =====================================================================

if __name__ == "__main__":
    stm = STM32Serial(port="/dev/ttyS0", baudrate=115200)

    print("GET_T  →", stm.get_temperature())
    time.sleep(1)
    print("GET_P  →", stm.get_pressure())
    time.sleep(1)
    print("GET_K  →", stm.get_k())
    time.sleep(1)
    print("SET_K  →", stm.set_k(1234))
    time.sleep(1)
    print("GET_A  →", stm.get_angle())

    stm.close()





