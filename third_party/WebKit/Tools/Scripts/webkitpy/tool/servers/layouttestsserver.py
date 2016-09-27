# Copyright (c) 2010 Google Inc. All rights reserved.
# Copyright (C) 2014 Samsung Electronics. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#     * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
#     * Neither the name of Google Inc. nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import time
import json
import BaseHTTPServer
import subprocess
from subprocess import Popen, PIPE, STDOUT

from webkitpy.common.webkit_finder import WebKitFinder
from webkitpy.common.system.filesystem import FileSystem
from webkitpy.tool.servers.reflectionhandler import ReflectionHandler


class LayoutTestsHTTPServer(BaseHTTPServer.HTTPServer):
    def __init__(self, httpd_port, config):
        server_name = ""
        server_address = ("", httpd_port)
        BaseHTTPServer.HTTPServer.__init__(self, server_address, LayoutTestsServerHTTPRequestHandler)


class LayoutTestsServerHTTPRequestHandler(ReflectionHandler):

    def do_POST(self):
        json_raw_data = self.rfile.read(int(self.headers.getheader('content-length')))
        json_data = json.loads(json_raw_data)
        test_list = ''
        for each in json_data['tests']:
            test_list += each + ' '
        filesystem = FileSystem()
        webkit_finder = WebKitFinder(filesystem)
        script_dir = webkit_finder.path_from_webkit_base('Tools', 'Scripts')
        executable_path = script_dir + "/run-webkit-tests"
        cmd = "python " + executable_path + " --no-show-results "
        cmd += test_list
        process = subprocess.Popen(cmd, shell=True, cwd=script_dir, env=None, stdout=subprocess.PIPE, stderr=STDOUT)
        self.send_response(200)
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header("Content-type", "text/html")
        self.end_headers()
        while process.poll() is None:
            html_output = '<br>' + str(process.stdout.readline())
            self.wfile.write(html_output)
            self.wfile.flush()
            time.sleep(0.05)
        process.wait()

    def do_OPTIONS(self):
        self.send_response(200, "ok")
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, POST, OPTIONS')
        self.send_header("Access-Control-Allow-Headers", "Content-type")
