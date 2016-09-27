# Copyright 2015 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""Start and stop the WPTserve servers as they're used by the layout tests."""

from webkitpy.layout_tests.servers import server_base


class WPTServe(server_base.ServerBase):
    def __init__(self, port_obj, output_dir):
        super(WPTServe, self).__init__(port_obj, output_dir)
        # These ports must match wpt_support/wpt.config.json
        http_port, http_alt_port, https_port = (8001, 8081, 8444)
        ws_port, wss_port = (9001, 9444)
        self._name = 'wptserve'
        self._log_prefixes = ('access_log', 'error_log')
        self._mappings = [{'port': http_port},
                          {'port': http_alt_port},
                          {'port': https_port, 'sslcert': True},
                          {'port': ws_port},
                          {'port': wss_port, 'sslcert': True}]

        # TODO(burnik): We can probably avoid PID files for WPT in the future.
        fs = self._filesystem
        self._pid_file = fs.join(self._runtime_path, '%s.pid' % self._name)

        path_to_thirdparty = self._port_obj.path_from_webkit_base('Tools', 'Scripts', 'webkitpy', 'thirdparty')
        path_to_wpt_support = self._port_obj.path_from_webkit_base('Tools', 'Scripts', 'webkitpy', 'thirdparty', 'wpt')
        path_to_wpt_root = fs.join(path_to_wpt_support, 'wpt')
        path_to_wpt_config = fs.join(path_to_wpt_support, 'wpt.config.json')
        path_to_wpt_tests = fs.abspath(fs.join(self._port_obj.layout_tests_dir(), 'imported', 'web-platform-tests'))
        path_to_ws_handlers = fs.join(path_to_wpt_tests, 'websockets', 'handlers')
        serve_script = fs.join(path_to_wpt_root, 'serve')
        start_cmd = [self._port_obj.host.executable,
                     '-u', serve_script,
                     '--config', path_to_wpt_config,
                     '--doc_root', path_to_wpt_tests]

        # TODO(burnik): Merge with default start_cmd once we roll in websockets.
        if self._port_obj.host.filesystem.exists(path_to_ws_handlers):
            start_cmd += ['--ws_doc_root', path_to_ws_handlers]

        self._stdout = self._stderr = self._executive.DEVNULL
        # TODO(burnik): We should stop setting the CWD once WPT can be run without it.
        self._cwd = path_to_wpt_root
        self._env = {'PYTHONPATH': path_to_thirdparty}
        self._keep_process_reference = True
        self._start_cmd = start_cmd

    def _stop_running_server(self):
        # Clean up the pid file.
        if self._pid and not self._executive.check_running_pid(self._pid):
            self._filesystem.remove(self._pid_file)
            return

        # TODO(burnik): Figure out a cleaner way of stopping wptserve.
        self._executive.interrupt(self._pid)

        # According to Popen.wait(), this can deadlock when using stdout=PIPE and/or stderr=PIPE.
        # We're using DEVNULL for both so that should not occur.
        self._process.wait()
