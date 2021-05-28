# Copyright PA Knowledge Ltd 2021
# MIT License. For licence terms see LICENCE.md file.

import unittest
import subprocess
import time
import os
import signal
import glob


class IntegrationTestsEnterpriseDiode(unittest.TestCase):
    def tearDown(self):
        [os.remove(file) for file in glob.glob("test_file.bin*") if os.path.isfile(file)]
        [os.remove(file) for file in glob.glob("received*") if os.path.isfile(file)]
        [os.remove(file) for file in glob.glob(".received.*") if os.path.isfile(file)]

    @staticmethod
    def write_bytes(filepath):
        with open(filepath, "wb") as file:
            return file.write(b"0" * int(110 * 1024))

    def send_file_with_ED_loopback(self, file_to_send):
        return self.run_ED_program("tester", file_to_send, "-c 5000 -s 5000", 1000)

    def send_file_with_ED_client(self, file_to_send):
        return self.run_ED_program("client", file_to_send, "-c 5000", 1000)

    @staticmethod
    def run_ED_program(ed_program, file_to_send, port_args, mtu_size=1000):
        return subprocess.Popen(
            f"cmake-build-release/{ed_program} -f {file_to_send} -a localhost {port_args} -m {mtu_size}".split())

    def wait_for_received_data(self, num_expect_files=1):
        attempts = 10
        all_data_received = False
        client_file_size = self.read_bytes("test_file.bin")
        files = []

        while (not all_data_received) or (len(files) < num_expect_files):
            files = glob.glob("test_file.bin.*")
            all_data_received = True
            for file in files:
                all_data_received &= (self.read_bytes(file) == client_file_size)

            time.sleep(1)
            attempts -= 1
            if not attempts:
                raise TimeoutError("Timed out waiting for received data")
        return True

    @staticmethod
    def read_bytes(filepath):
        with open(filepath, "rb") as file:
            return file.read()

    def test_file_is_sent_and_saved_with_ed_loopback(self):
        self.write_bytes("test_file.bin")
        handle = self.send_file_with_ED_loopback(file_to_send="test_file.bin")
        self.assertTrue(self.wait_for_received_data())
        handle.send_signal(signal.SIGINT)
        self.assertEqual(handle.wait(timeout=5), 0)

    def test_ed_client_sends_file(self):
        self.write_bytes("test_file.bin")
        self.assertEqual(self.send_file_with_ED_client(file_to_send="test_file.bin").wait(timeout=5), 0)

    def test_error_returned_if_file_not_present(self):
        self.assertEqual(self.send_file_with_ED_loopback(file_to_send="my_file_not_present.bin").wait(timeout=5), 2)
        self.assertEqual(self.send_file_with_ED_client(file_to_send="my_file_not_present.bin").wait(timeout=5), 2)

    @staticmethod
    def wait_for_server_start(port, attempts=10):
        while subprocess.call(f"lsof -i :{port}".split()):
            time.sleep(1)
            attempts -= 1
            if attempts == 0:
                raise TimeoutError("Server failed to start")

    def start_ED_server_thread(self):
        process_handle = self.run_ED_server(5000)
        self.wait_for_server_start(port=5000)
        return process_handle

    @staticmethod
    def run_ED_server(port):
        return subprocess.Popen(f"cmake-build-release/server -s {port}".split())

    def test_wait_for_ed_server_start_does_timeout(self):
        self.assertRaises(TimeoutError, self.wait_for_server_start, 5001, attempts=1)

    def test_ed_server_receives_file(self):
        self.write_bytes("test_file.bin")
        server_handle = self.start_ED_server_thread()
        self.assertEqual(self.send_file_with_ED_client(file_to_send="test_file.bin").wait(timeout=5), 0)
        self.assertTrue(self.wait_for_received_data())
        server_handle.send_signal(signal.SIGINT)
        self.assertEqual(server_handle.wait(timeout=5), 0)

    def test_ed_server_receives_multiple_files(self):
        self.write_bytes("test_file.bin")
        server_handle = self.start_ED_server_thread()

        for i in range(0, 4, 1):
            self.assertEqual(self.send_file_with_ED_client(file_to_send="test_file.bin").wait(timeout=5), 0)

        self.assertTrue(self.wait_for_received_data(num_expect_files=4))
        server_handle.send_signal(signal.SIGINT)
        self.assertEqual(server_handle.wait(timeout=5), 0)


if __name__ == '__main__':
    SUITE = unittest.TestLoader().loadTestsFromTestCase(IntegrationTestsEnterpriseDiode)
    unittest.TextTestRunner(verbosity=5).run(SUITE)
