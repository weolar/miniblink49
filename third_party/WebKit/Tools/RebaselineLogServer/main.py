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

import datetime
import json
import logging
import sys
import webapp2

from google.appengine.api import mail
from google.appengine.ext import ndb
from google.appengine.ext.webapp import template
from google.appengine.ext.db import BadRequestError

# A simple log server for rebaseline-o-matic.
#
# Accepts updates to the same log entry and shows a simple status page.
# Has a special state for the case where there are no NeedsRebaseline
# lines in TestExpectations to avoid cluttering the log with useless
# entries every 30 seconds.
#
# Other than that, new updatelog calls append to the most recent log
# entry until they have the newentry parameter, in which case, it
# starts a new log entry.

LOG_PARAM = "log"
NEW_ENTRY_PARAM = "newentry"
NUM_LOGS_PARAM = "numlogs"
BEFORE_PARAM = "before"
COMMAND_PARAM = "command"

SENDER_ADDRESS = "bot@blinkrebaseline.appspotmail.com"
REBASELINES_LIST_ADDRESS = "blink-rebaselines@chromium.org"

class LogEntry(ndb.Model):
    content = ndb.TextProperty()
    date = ndb.DateTimeProperty(auto_now_add=True)


class SendMailCommand(object):
    def execute(self, command_data):
        if "to" not in command_data or "subject" not in command_data or "body" not in command_data:
            return "Malformed command JSON"
        to = command_data["to"]
        subject = command_data["subject"]
        body = command_data["body"]
        htmlbody = command_data["htmlbody"] if "htmlbody" in command_data else body
        mail.send_mail(sender=SENDER_ADDRESS,
                       to=to,
                       reply_to="@".join(["apavlov", "chromium.org"]),
                       cc=REBASELINES_LIST_ADDRESS,
                       subject="Auto-rebaseline bot: " + subject,
                       body=body,
                       html=htmlbody)
        return "Sent mail to %s (%s)" % (to, subject)


COMMANDS = {
    "sendmail": SendMailCommand()
}


def logs_query():
    return LogEntry.query().order(-LogEntry.date)


def execute_command(command_data):
    if not command_data:
        return "ERROR: No command data"

    if "name" not in command_data:
        return "Command is missing a name: %s" % json.dumps(command_data)

    name = command_data["name"]
    command = COMMANDS.get(name)
    if not command:
        return "Command %s is unknown"
    try:
        return command.execute(command_data)
    except Exception as e:
        return "Exception caught when executing command '%s'" % name


class UpdateLog(webapp2.RequestHandler):
    def post(self):
        new_log_data = self.request.POST.get(LOG_PARAM)

        command_json = self.request.POST.get(COMMAND_PARAM)
        command_data = json.loads(command_json) if command_json else None

        command_output = ""
        if command_data is not None:
            command_output = "\n" + execute_command(command_data)
            new_log_data += command_output

        # This entry is set to on whenever a new auto-rebaseline run is going to
        # start logging entries. If this is not on, then the log will get appended
        # to the most recent log entry.
        new_entry = self.request.POST.get(NEW_ENTRY_PARAM) == "on"

        out = "Wrote new log entry."
        if not new_entry:
            log_entries = logs_query().fetch(1)
            if log_entries:
                log_entry = log_entries[0]
                log_entry.date = datetime.datetime.now()
                log_entry.content = log_entry.content + "\n" + new_log_data
                out = "Added to existing log entry."

        if new_entry or not log_entries:
            log_entry = LogEntry(content=new_log_data)

        try:
            log_entry.put()
        except BadRequestError:
            out = "Created new log entry because the previous one exceeded the max length."
            LogEntry(content=new_log_data, date=datetime.datetime.now()).put()

        self.response.headers.add_header("Content-Type", "text/plain")
        self.response.out.write(out + command_output)


class UploadForm(webapp2.RequestHandler):
    def get(self):
        self.response.out.write(template.render("uploadform.html", {
            "update_log_url": "/updatelog",
            "set_no_needs_rebaseline_url": "/noneedsrebaselines",
            "log_param": LOG_PARAM,
            "new_entry_param": NEW_ENTRY_PARAM,
            "command_param": COMMAND_PARAM
        }))


class ShowLatest(webapp2.RequestHandler):
    def get(self):
        query = logs_query()

        before = self.request.get(BEFORE_PARAM)
        if before:
            date = datetime.datetime.strptime(before, "%Y-%m-%dT%H:%M:%SZ")
            query = query.filter(LogEntry.date < date)

        num_logs = self.request.get(NUM_LOGS_PARAM)
        logs = query.fetch(int(num_logs) if num_logs else 3)

        self.response.out.write(template.render("logs.html", {
            "logs": logs,
            "num_logs_param": NUM_LOGS_PARAM,
            "before_param": BEFORE_PARAM,
        }))


routes = [
    ('/uploadform', UploadForm),
    ('/updatelog', UpdateLog),
    ('/', ShowLatest),
]

app = webapp2.WSGIApplication(routes, debug=True)
