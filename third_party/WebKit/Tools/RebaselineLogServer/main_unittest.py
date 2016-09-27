# Copyright (C) 2013 Google Inc. All rights reserved.
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

# Usage: PYTHONPATH=/path/to/appengine_sdk python main_unittest.py.

import dev_appserver
dev_appserver.fix_sys_path()

import json
import unittest
import webapp2

from google.appengine.ext import testbed

import main


class TestHandlers(unittest.TestCase):
    def setUp(self):
        self.testbed = testbed.Testbed()
        self.testbed.activate()
        self.testbed.init_datastore_v3_stub()
        self.testbed.init_memcache_stub()
        self.testbed.init_mail_stub()

    def _new_request(self, data, command=None):
        request = webapp2.Request.blank('/updatelog')
        request.method = 'POST'
        request.POST[main.LOG_PARAM] = data
        request.POST[main.NEW_ENTRY_PARAM] = 'off'
        if command is not None:
            request.POST[main.COMMAND_PARAM] = json.dumps(command)
        return request

    def _assert_response(self, response, body):
        self.assertEqual(response.status_int, 200)
        self.assertEqual(response.body, body)

    def test_update_log(self):
        request = self._new_request('data to log')
        request.POST[main.NEW_ENTRY_PARAM] = 'on'
        self._assert_response(request.get_response(main.app), 'Wrote new log entry.')

        self._assert_response(request.get_response(main.app), 'Wrote new log entry.')

        request = self._new_request('data to log')
        self._assert_response(request.get_response(main.app), 'Added to existing log entry.')

        request = self._new_request('x' * 1000000)
        self._assert_response(request.get_response(main.app), 'Created new log entry because the previous one exceeded the max length.')

        request = self._new_request('data to log')
        self._assert_response(request.get_response(main.app), 'Created new log entry because the previous one exceeded the max length.')

        request = self._new_request('data to log')
        self._assert_response(request.get_response(main.app), 'Added to existing log entry.')

    def test_command_execution(self):
        request = self._new_request('data to log', {})
        self._assert_response(request.get_response(main.app), 'Wrote new log entry.\nERROR: No command data')

        request = self._new_request('data to log', {"foo": "bar"})
        self._assert_response(request.get_response(main.app), 'Added to existing log entry.\nCommand is missing a name: {"foo": "bar"}')

        request = self._new_request('data to log', {
            'name': 'sendmail',
            'to': 'foo@example.com',
            'body': 'Body'
        })
        self._assert_response(request.get_response(main.app), 'Added to existing log entry.\nMalformed command JSON')

        request = self._new_request('data to log', {
            'name': 'sendmail',
            'to': 'foo@example.com',
            'subject': 'Subject',
            'body': 'Body'
        })
        self._assert_response(request.get_response(main.app), 'Added to existing log entry.\nSent mail to foo@example.com (Subject)')

    def test_update_log_first_entry_without_new_entry_param(self):
        request = webapp2.Request.blank('/updatelog')
        request.method = 'POST'
        request.POST[main.LOG_PARAM] = 'data to log'
        request.POST[main.NEW_ENTRY_PARAM] = 'off'

        response = request.get_response(main.app)
        self.assertEqual(response.status_int, 200)
        self.assertEqual(response.body, 'Wrote new log entry.')

    def test_update_log_first_entry_no_needs_rebaseline_param(self):
        request = webapp2.Request.blank('/updatelog')
        request.method = 'POST'
        request.POST[main.LOG_PARAM] = 'data to log'
        request.POST[main.NEW_ENTRY_PARAM] = 'off'

        response = request.get_response(main.app)
        self.assertEqual(response.status_int, 200)
        self.assertEqual(response.body, 'Wrote new log entry.')


if __name__ == '__main__':
    unittest.main()
