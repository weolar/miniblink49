package org.chromium.devtools.compiler;

import com.google.common.collect.Lists;
import com.google.common.io.Resources;
import com.google.javascript.jscomp.*;

import org.kohsuke.args4j.CmdLineException;
import org.kohsuke.args4j.CmdLineParser;
import org.kohsuke.args4j.Option;

import javax.xml.transform.Source;
import java.io.*;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Prepares and executes several instances of the closure compiler.
 */
public class Runner {
    protected final Flags flags = new Flags();
    private final PrintStream err;
    private boolean isConfigValid;

    public Runner(String[] args, PrintStream err) {
        this.err = err;
        List<String> argList = processArgs(args);
        CmdLineParser parser = new CmdLineParser(flags);
        isConfigValid = true;
        try {
            parser.parseArgument(argList.toArray(new String[] {}));
            if (flags.compilerArgsFile == null) {
                isConfigValid = false;
            }
        } catch (CmdLineException e) {
            err.println(e.getMessage());
            isConfigValid = false;
        }

        if (!isConfigValid) {
            parser.printUsage(err);
        }
    }

    private List<String> processArgs(String[] args) {
        Pattern argPattern = Pattern.compile("(--[a-zA-Z_]+)=(.*)");
        Pattern quotesPattern = Pattern.compile("^['\"](.*)['\"]$");
        List<String> processedArgs = Lists.newArrayList();

        for (String arg : args) {
            Matcher matcher = argPattern.matcher(arg);
            if (matcher.matches()) {
                processedArgs.add(matcher.group(1));

                String value = matcher.group(2);
                Matcher quotesMatcher = quotesPattern.matcher(value);
                if (quotesMatcher.matches()) {
                    processedArgs.add(quotesMatcher.group(1));
                } else {
                    processedArgs.add(value);
                }
            } else {
                processedArgs.add(arg);
            }
        }

        return processedArgs;
    }

    private boolean shouldRunCompiler() {
        return isConfigValid;
    }

    protected void logError(String message, Exception e) {
        err.println("ERROR: " + message);
        if (e != null) {
            e.printStackTrace(err);
        }
    }

    private void run() {
        List<CompilerInstanceDescriptor> descriptors = getDescriptors();
        if (descriptors == null) {
            return;
        }
        ExecutorService executor = Executors.newFixedThreadPool(
                Math.min(descriptors.size(), Runtime.getRuntime().availableProcessors() / 2 + 1));
        try {
            runWithExecutor(descriptors, executor);
        } finally {
            executor.shutdown();
        }
    }

    private void runWithExecutor(
            List<CompilerInstanceDescriptor> descriptors, ExecutorService executor) {
        List<Future<CompilerRunner>> futures = new ArrayList<>(descriptors.size());
        for (CompilerInstanceDescriptor descriptor : descriptors) {
            CompilerRunner task = new CompilerRunner(descriptor, new ByteArrayOutputStream(512));
            futures.add(executor.submit(task));
        }

        for (Future<CompilerRunner> future : futures) {
            try {
                CompilerRunner task = future.get();
                int result = task.result;
                if (result != 0) {
                    System.err.println("ERROR: Compiler returned " + result);
                }
                task.errStream.flush();
                System.err.println("@@ START_MODULE:" + task.descriptor.moduleName + " @@");
                System.err.println(task.errStream.toString("UTF-8"));
                System.err.println("@@ END_MODULE @@");
            } catch (Exception e) {
                System.err.println("ERROR - " + e.getMessage());
                e.printStackTrace(System.err);
            }
        }
        System.exit(0);
    }

    private List<CompilerInstanceDescriptor> getDescriptors() {
        List<CompilerInstanceDescriptor> result = new ArrayList<>();
        try (BufferedReader reader = new BufferedReader(
                new InputStreamReader(
                        new FileInputStream(flags.compilerArgsFile), "UTF-8"))) {
            int lineIndex = 0;
            while (true) {
                ++lineIndex;
                String line = reader.readLine();
                if (line == null) {
                    break;
                }
                if (line.length() == 0) {
                    continue;
                }
                String[] moduleAndArgs = line.split(" +", 2);
                if (moduleAndArgs.length != 2) {
                    logError(String.format(
                            "Line %d does not contain module name and compiler arguments",
                            lineIndex), null);
                    continue;
                }
                result.add(new CompilerInstanceDescriptor(moduleAndArgs[0], moduleAndArgs[1]));
            }
        } catch (IOException e) {
            logError("Failed to read compiler arguments file", e);
            return null;
        }

        return result;
    }

    public static void main(String[] args) {
        Runner runner = new Runner(args, System.err);
        if (runner.shouldRunCompiler()) {
            runner.run();
        } else {
            System.exit(-1);
        }
    }

    private static class LocalCommandLineRunner extends CommandLineRunner {

        private static final String EXCLUDED_EXTERNS_ES6_JS = "externs.zip//es6.js";

        protected LocalCommandLineRunner(String[] args, PrintStream out, PrintStream err) {
            super(args, out, err);
        }

        @Override
        protected List<SourceFile> createExterns() throws FlagUsageException, IOException {
            List<SourceFile> externs = super.createExterns();
            ArrayList<SourceFile> result = new ArrayList<>(externs.size());
            for (SourceFile sourceFile: externs) {
                if (!EXCLUDED_EXTERNS_ES6_JS.equals(sourceFile.getName())) {
                    result.add(sourceFile);
                }
            }
            return result;
        }

        @Override
        protected CompilerOptions createOptions() {
            CompilerOptions options = super.createOptions();
            options.setIdeMode(true);
            options.setReportMissingOverride(CheckLevel.ERROR);
            return options;
        }

        @Override
        protected void setRunOptions(CompilerOptions options)
                throws FlagUsageException, IOException {
            super.setRunOptions(options);
            options.setCodingConvention(new DevToolsCodingConvention());
        }

        int execute() {
            int result = 0;
            int runs = 1;
            try {
                for (int i = 0; i < runs && result == 0; i++) {
                    result = doRun();
                }
            } catch (Throwable t) {
                t.printStackTrace();
                result = -2;
            }
            return result;
        }
    }

    private static class CompilerRunner implements Callable<CompilerRunner> {
        private final CompilerInstanceDescriptor descriptor;
        private final ByteArrayOutputStream errStream;
        private int result;

        public CompilerRunner(
                CompilerInstanceDescriptor descriptor, ByteArrayOutputStream errStream) {
            this.descriptor = descriptor;
            this.errStream = errStream;
        }

        @Override
        public CompilerRunner call() throws Exception {
            PrintStream errPrintStream = new PrintStream(errStream, false, "UTF-8");
            LocalCommandLineRunner runner =
                    new LocalCommandLineRunner(prepareArgs(), System.out, errPrintStream);
            if (!runner.shouldRunCompiler()) {
                this.result = -1;
            }
            this.result = runner.execute();
            return this;
        }

        private String[] prepareArgs() {
            // FIXME: This does not support quoted arguments.
            return descriptor.commandLine.split(" +");
        }
    }

    private static class Flags {
        @Option(name = "--compiler-args-file",
                usage = "Full path to file containing compiler arguments (one line per instance)")
        private String compilerArgsFile = null;
    }

    private static class CompilerInstanceDescriptor {
        private final String moduleName;
        private final String commandLine;

        public CompilerInstanceDescriptor(String moduleName, String commandLine) {
            this.moduleName = moduleName;
            this.commandLine = commandLine;
        }
    }
}
