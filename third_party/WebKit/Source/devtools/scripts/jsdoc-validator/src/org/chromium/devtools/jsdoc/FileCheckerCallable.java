package org.chromium.devtools.jsdoc;

import com.google.javascript.jscomp.Compiler;
import com.google.javascript.jscomp.NodeTraversal;
import com.google.javascript.jscomp.parsing.Config;
import com.google.javascript.jscomp.parsing.Config.LanguageMode;
import com.google.javascript.jscomp.parsing.ParserRunner;
import com.google.javascript.rhino.ErrorReporter;
import com.google.javascript.rhino.Node;

import org.chromium.devtools.jsdoc.checks.ContextTrackingValidationCheck;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.nio.file.FileSystems;
import java.nio.file.Files;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.concurrent.Callable;

public class FileCheckerCallable implements Callable<ValidatorContext> {

    private static Set<String> EXTRA_ANNOTATIONS = new HashSet<>(Arrays.asList(
        "suppressReceiverCheck",
        "suppressGlobalPropertiesCheck"
    ));
    private final String fileName;

    public FileCheckerCallable(String fileName) {
        this.fileName = fileName;
    }

    @Override
    public ValidatorContext call() {
        try {
            ValidatorContext context = new ValidatorContext(readScriptText(), fileName);
            ValidationCheckDispatcher dispatcher = new ValidationCheckDispatcher(context);
            dispatcher.registerCheck(new ContextTrackingValidationCheck());
            NodeTraversal.traverse(new Compiler(), parseScript(context), dispatcher);
            return context;
        } catch (FileNotFoundException e) {
            logError("File not found: " + fileName);
        } catch (IOException e) {
            logError("Failed to read file " + fileName);
        }
        return null;
    }

    private String readScriptText() throws IOException {
        byte[] encoded = Files.readAllBytes(FileSystems.getDefault().getPath(fileName));
        String text = StandardCharsets.UTF_8.decode(ByteBuffer.wrap(encoded)).toString();
        return text;
    }

    private static Node parseScript(final ValidatorContext context) {
        Config config = ParserRunner.createConfig(
            true, LanguageMode.ECMASCRIPT5_STRICT, true, EXTRA_ANNOTATIONS);
        ErrorReporter errorReporter = new ErrorReporter() {
            @Override
            public void warning(String message, String sourceName, int line, int lineOffset) {
                // Ignore.
            }

            @Override
            public void error(String message, String sourceName, int line, int lineOffset) {
                logError("at " + sourceName + ":" + line + ":" + lineOffset);
            }
        };
        try {
            return ParserRunner.parse(
                    context.sourceFile, context.sourceFile.getCode(), config, errorReporter).ast;
        } catch (IOException e) {
            // Does not happen with preloaded files.
            return null;
        }
    }

    private static void logError(String message) {
        System.err.println("ERROR: " + message);
    }

    private static class ValidationCheckDispatcher extends DoDidVisitorAdapter {
        private final List<ValidationCheck> checks = new ArrayList<>(2);
        private final ValidatorContext context;

        public ValidationCheckDispatcher(ValidatorContext context) {
            this.context = context;
        }

        public void registerCheck(ValidationCheck check) {
            check.setContext(context);
            checks.add(check);
        }

        @Override
        public void doVisit(Node node) {
            for (DoDidNodeVisitor visitor : checks) {
                visitor.doVisit(node);
            }
        }

        @Override
        public void didVisit(Node node) {
            for (ValidationCheck check : checks) {
                check.didVisit(node);
            }
        }
    }
}
