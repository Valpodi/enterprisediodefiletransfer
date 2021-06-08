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
        [os.remove(file) for file in glob.glob("cmake-build-release/files_to_send/*") if os.path.isfile(file)]
        [os.remove(file) for file in glob.glob("cmake-build-release/files_received/*") if os.path.isfile(file)]
        [os.remove(file) for file in glob.glob("cmake-build-release/files_to_send/nested/*") if os.path.isfile(file)]
        if os.path.isdir("cmake-build-release/files_to_send/nested"):
            os.rmdir("cmake-build-release/files_to_send/nested")
        [os.remove(file) for file in glob.glob(".received.*") if os.path.isfile(file)]

    @classmethod
    def tearDownClass(cls):
        os.rmdir("cmake-build-release/files_to_send")
        os.rmdir("cmake-build-release/files_received")

    @classmethod
    def setUpClass(cls):
        if not os.path.isdir("cmake-build-release/files_to_send"):
            os.mkdir("cmake-build-release/files_to_send")
        if not os.path.isdir("cmake-build-release/files_received"):
            os.mkdir("cmake-build-release/files_received")

    @staticmethod
    def write_bytes(filepath):
        with open(f"cmake-build-release/files_to_send/{filepath}", "wb") as file:
            return file.write(b"0" * int(110 * 1024))

    def send_file_with_ED_loopback(self, file_to_send):
        return self.run_ED_program("tester", file_to_send, "-c 5000 -s 5000", 1000)

    def send_file_with_ED_client(self, file_to_send):
        return self.run_ED_program("client", file_to_send, "-c 5000", 1000)

    @staticmethod
    def run_ED_program(ed_program, file_to_send, port_args, mtu_size=1000):
        os.chdir("cmake-build-release/files_to_send")
        process = subprocess.Popen(
            f"../{ed_program} -f {file_to_send} -a localhost {port_args} -m {mtu_size}".split())
        os.chdir("../..")
        return process

    def wait_for_received_data(self, num_expect_files=1, output_dir="cmake-build-release/files_received", input_file="test_file.bin"):
        attempts = 10
        all_data_received = False
        client_file_size = self.read_bytes(f"cmake-build-release/files_to_send/{input_file}")
        files = []

        while (not all_data_received) or (len(files) < num_expect_files):
            files = os.listdir(output_dir)
            all_data_received = True
            for file in files:
                all_data_received &= (self.read_bytes(os.path.join(output_dir, file)) == client_file_size)

            time.sleep(1)
            attempts -= 1
            if not attempts:
                raise TimeoutError("Timed out waiting for received data")
        return True

    @staticmethod
    def read_bytes(filepath):
        with open(filepath, "rb") as file:
            return file.read()

    @staticmethod
    def is_files_received_dir_is_empty():
        return not os.listdir("cmake-build-release/files_received/")

    def test_file_is_sent_and_saved_with_ed_loopback(self):
        self.write_bytes("test_file.bin")
        handle = self.send_file_with_ED_loopback(file_to_send="test_file.bin")
        self.assertTrue(self.wait_for_received_data(output_dir="cmake-build-release/files_to_send"))
        handle.send_signal(signal.SIGINT)
        self.assertEqual(handle.wait(timeout=5), 0)

    def test_ed_client_sends_file(self):
        self.write_bytes("test_file.bin")
        self.assertEqual(self.send_file_with_ED_client(file_to_send="test_file.bin").wait(timeout=5), 0)

    def test_error_returned_if_file_not_present(self):
        self.assertEqual(self.send_file_with_ED_loopback(file_to_send="my_file_not_present.bin").wait(timeout=5), 2)
        self.assertEqual(self.send_file_with_ED_client(file_to_send="my_file_not_present.bin").wait(timeout=5), 2)

    def test_file_is_rejected_when_sent_with_illegal_filename(self):
        filename = "test_file?.bin"
        self.write_bytes(filename)
        self.assertRaises(Exception, self.send_file_with_ED_client(file_to_send=filename))

    @staticmethod
    def wait_for_server_start(port, attempts=10):
        while subprocess.call(f"lsof -i :{port}".split()):
            time.sleep(1)
            attempts -= 1
            if attempts == 0:
                raise TimeoutError("Server failed to start")

    def start_ED_server_thread(self, set_drop_packets_flag=False):
        process_handle = self.run_ED_server(5000, set_drop_packets_flag)
        self.wait_for_server_start(port=5000)
        return process_handle

    @staticmethod
    def run_ED_server(port, set_drop_packets_flag=False):
        os.chdir("cmake-build-release/files_received")
        server = subprocess.Popen(f"../server -s {port} {'-d' * set_drop_packets_flag}".split())
        os.chdir("../..")
        return server

    def test_wait_for_ed_server_start_does_timeout(self):
        self.assertRaises(TimeoutError, self.wait_for_server_start, 5001, attempts=1)

    def test_ed_server_receives_file(self):
        self.write_bytes("test_file.bin")
        server_handle = self.start_ED_server_thread()
        self.assertEqual(self.send_file_with_ED_client(file_to_send="test_file.bin").wait(timeout=5), 0)
        self.assertTrue(self.wait_for_received_data())
        server_handle.send_signal(signal.SIGINT)
        self.assertEqual(server_handle.wait(timeout=5), 0)

    def test_ed_server_receives_nested_file(self):
        os.mkdir("cmake-build-release/files_to_send/nested")
        self.write_bytes("nested/test_file.bin")
        server_handle = self.start_ED_server_thread()
        self.assertEqual(self.send_file_with_ED_client(file_to_send="nested/test_file.bin").wait(timeout=5), 0)
        self.assertTrue(self.wait_for_received_data(input_file="nested/test_file.bin"))
        server_handle.send_signal(signal.SIGINT)
        self.assertEqual(server_handle.wait(timeout=5), 0)

    def test_ed_server_receives_overwrites_files_that_exist(self):
        self.write_bytes("test_file.bin")
        server_handle = self.start_ED_server_thread()

        for i in range(0, 4):
            self.assertEqual(self.send_file_with_ED_client(file_to_send="test_file.bin").wait(timeout=5), 0)

        self.assertTrue(self.wait_for_received_data())
        self.assertTrue(len(os.listdir("cmake-build-release/files_received")) == 1)
        server_handle.send_signal(signal.SIGINT)
        self.assertEqual(server_handle.wait(timeout=5), 0)

    def test_ed_server_does_not_write_files_to_disk_if_correct_flag_is_set(self):
        self.write_bytes("test_file.bin")
        server_handle = self.start_ED_server_thread(set_drop_packets_flag=True)
        self.assertEqual(self.send_file_with_ED_client(file_to_send="test_file.bin").wait(timeout=5), 0)
        self.assertTrue(self.is_files_received_dir_is_empty())
        server_handle.send_signal(signal.SIGINT)
        self.assertEqual(server_handle.wait(timeout=5), 0)

if __name__ == '__main__':
    SUITE = unittest.TestLoader().loadTestsFromTestCase(IntegrationTestsEnterpriseDiode)
    unittest.TextTestRunner(verbosity=5).run(SUITE)
