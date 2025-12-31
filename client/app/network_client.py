import os
import socket
import threading
import ssl
import json

class NetworkClient:
    def __init__(self):
        self.client_socket = None
        self.is_connected = False

    def connect(self, host, port, callback):
        try:
            current_dir = os.path.dirname(os.path.abspath(__file__))
            cert_path = os.path.join(current_dir, "..", "..", "server", "resources", "server.crt")
            raw_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            context = ssl.create_default_context()        
            context.load_verify_locations(cert_path)    
            context.check_hostname = False
            context.verify_mode = ssl.CERT_REQUIRED
            self.client_socket = context.wrap_socket(raw_socket, server_hostname=host)
            
            self.client_socket.connect((host, port))
            self.is_connected = True
            
            thread = threading.Thread(target=self._receive_loop, args=(callback,), daemon=True)
            thread.start()
            return True
        except Exception as e:
            print(f"Błąd połączenia: {e}")
            return False

    def _receive_loop(self, callback):
        while self.is_connected:
            try:
                data = self.client_socket.recv(1024).decode('utf-8')
                if not data:
                    break
                callback(data)
            except:
                break
        self.disconnect()

    def send_message(self, message):
        if self.is_connected:
            try:
                self.client_socket.sendall(message.encode('utf-8'))
            except Exception as e:
                print(f"Błąd wysyłania: {e}")

    def disconnect(self):
        self.is_connected = False
        if self.client_socket:
            self.client_socket.close()

    def send_json(self, data_dict):
        if self.is_connected:
            try:
                message = json.dumps(data_dict) + "\n"
                self.client_socket.sendall(message.encode('utf-8'))
            except Exception as e:
                print(f"Błąd wysyłania JSON: {e}")