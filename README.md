# HRL (Human-Robot Language)

HRL is a language prototype specifically designed to help developers and product owners program AI-powered systems for a wide range of applications, including chatbots, virtual assistants, and physical robots. The language is designed to be easy to learn and use, with a clean and concise syntax that allows for rapid integration and development.

## Key Features

- **Easy Parallelization**: HRL provides built-in constructs for parallel execution and load balancing, allowing developers to easily distribute tasks across multiple machines or processes.

- **Pub/Sub Topics**: The language supports a publish/subscribe model for communication between different components of a system, where modules or components can publish messages (events) to specific topics and subscribe to receive messages from topics of interest.

- **Context Switching Mechanism**: Components can easily adapt to different contexts or environments by switching between different sets of rules or behaviors based on the current context or state of the system.

- **Setup-Loop by Default**: Programs by default run in an infinite main loop after setup, allowing for continuous execution and monitoring of the system.

- **Easy Integration**: HRL code can easily call scripts written in Python, C++ and other languages and capture their output. This makes it very easy to integrate existing models and libraries for common agent tasks (e.g., natural language processing, vision models, audio models, path planning, etc.), so you can focus on the higher-level logic.

## Example Code

```hrl
-- Setup block will be executed once at the beginning of execution
setup {
    -- Define context IDs
    enum Context {
        General,
        Task,
        ErrorHandling
    }

    -- Define session data structure
    struct Session {
        id: String,
        context: Context,
        -- Other session data
    }

    -- Define pub/sub topics
    enum Topic {
        UserInput,
        TaskCompleted,
        ErrorOccurred
    }

    -- Context-specific behaviors

    -- Behavior for the general context
    behavior generalContext(session: Session) {
        -- Listen for user input
        userInput = read();

        -- Publish user input to topic
        publishToTopic(Topic.UserInput, userInput);

        -- Process user input based on context
        if (userInput == "start task") {
            switchContext(session, Context.Task);
        } else if (userInput == "error") {
            switchContext(session, Context.ErrorHandling);
        } else {
            respond(session.id, "Sorry, I didn't understand.");
        }
    }

    -- Behavior for the task context
    behavior taskContext(session: Session) {
        -- Perform task-specific operations
        performTask(session.id);

        -- Publish task completion to topic
        publishToTopic(Topic.TaskCompleted, session.id);

        -- Listen for user input to switch context
        const userInput = waitForUserInput(session.id);
        if (userInput == "exit") {
            switchContext(session, Context.General);
        }
    }

    -- Behavior for the error handling context
    behavior errorHandlingContext(session: Session) {
        -- Handle errors
        handleError(session.id);

        -- Publish error to topic
        publishToTopic(Topic.ErrorOccurred, session.id);

        -- Listen for user input to switch context
        const userInput = waitForUserInput(session.id);
        if (userInput == "retry") {
            switchContext(session, Context.General);
        }
    }

    -- Start pub/sub system
    startPubSubSystem();
}

-- Main block will be executed in an infinite loop after setup
main {
    -- Create sessions for each unattended chat
    sessions = createSessions();

    -- Start context-specific behaviors for each session in parallel
    while (!sessions.isEmpty()) {
        session = sessions.pop();
        threadloop startBehavior(generalContext, session);
        threadloop startBehavior(taskContext, session);
        threadloop startBehavior(errorHandlingContext, session);
    }
}
```

## Integrations

The following subscript demonstrates how the language can be used to integrate different services and other scripts. 

In this example, the language is used to create a simple chatbot that can detect objects in images and generate descriptions for them using AI models and text-to-speech services, by calling other scripts with the necessary OpenCV, MetaAISAM and Google Cloud Text-to-Speech libraries/plugins (whose terminal output is captured and returned as a string).

This makes HRL a good tool for blueprinting and orchestrating complex AI systems, as it can easily call and integrate existing models and libraries for common agent tasks (e.g., natural language processing, vision models, audio models, path planning, etc.), so you can focus on the higher-level logic.

```hrl
behavior objectDetectionContext(session: Session) {
    image = callprogram("./capture_image");
    objects = callprogram("detect_objects_meta_sam.py", image);
    publishToTopic(Topic.ObjectDetected, objects);
    switchContext(session, Context.Description);
}

behavior descriptionContext(session: Session) {
    objects = waitForMessage(Topic.ObjectDetected);
    description = generateDescription(objects);
    callprogram("google_cloud_tts.lua", description, "EN");
    publishToTopic(Topic.DescriptionCompleted, session.id);
    switchContext(session, Context.Idle);
}

behavior idleContext(session: Session) {
    trigger = read();
    if (trigger == "capture") {
        switchContext(session, Context.ObjectDetection);
    }
}

function generateDescription(objects: List<Object>): String {
    description = "Detected objects: ";
    while (!objects.isEmpty()) {
        object = objects.pop();
        description += object.name + ", ";
    }
    return description.trim();
}

function waitForImageCaptureTrigger(sessionId: String): String {
    return callprogram("wait_for_trigger.py", sessionId);
}
```

## EBNF (Extended Backus-Naur Form) Grammar

<li>PROGRAM = SETUP, MAIN;
<li>SETUP = "setup", BLOCK;
<li>MAIN = "main", BLOCK;
<li>STATEMENTS = (VARIABLE_DECLARATION | ENUM_DECLARATION | STRUCT_DECLARATION | BEHAVIOR_DECLARATION | WHILE_STATEMENT | IF_STATEMENT | FUNCTION_CALL_STATEMENT | FUNCTION_DECLARATION | CONST_DECLARATION | THREADLOOP_STATEMENT | BREAK_STATEMENT | CONTINUE_STATEMENT | RETURN_STATEMENT), ";";
<li>VARIABLE_DECLARATION = IDENTIFIER, ":", TYPE, [ARRAY_SPECIFIER], "=", (EXPRESSION | ARRAY_INITIALIZER);
<li>ARRAY_SPECIFIER = "[", EXPRESSION, "]";
<li>ARRAY_INITIALIZER = "[", EXPRESSION, { ",", EXPRESSION }, "]";
<li>ENUM_DECLARATION = "enum", IDENTIFIER, "{", ENUM_MEMBER, { ",", ENUM_MEMBER }, "}";
<li>ENUM_MEMBER = IDENTIFIER;
<li>STRUCT_DECLARATION = "struct", IDENTIFIER, "{", VARIABLE_DECLARATION, { ",", VARIABLE_DECLARATION }, "}";
<li>BEHAVIOR_DECLARATION = "behavior", IDENTIFIER, "(", PARAMETERS, ")", BLOCK;
<li>PARAMETERS = [PARAMETER, { ",", PARAMETER }];
<li>PARAMETER = IDENTIFIER, ":", TYPE;
<li>BLOCK = "{", {STATEMENTS}, "}";
<li>WHILE_STATEMENT = "while", "(", EXPRESSION, ")", BLOCK;
<li>IF_STATEMENT = "if", "(", EXPRESSION, ")", BLOCK, ["else", BLOCK];
<li>CASE_BLOCK = "case", EXPRESSION, ":", STATEMENTS;
<li>FUNCTION_CALL_STATEMENT = IDENTIFIER, "(", [ARGUMENTS], ")";
<li>ARGUMENTS = [EXPRESSION, { ",", EXPRESSION }];
<li>FUNCTION_DECLARATION = "function", IDENTIFIER, "(", PARAMETERS, ")", ":", TYPE, BLOCK;
<li>CONST_DECLARATION = "const", IDENTIFIER, "=", EXPRESSION;
<li>THREADLOOP_STATEMENT = "threadloop", IDENTIFIER, "(", ARGUMENTS, ")";
<li>BREAK_STATEMENT = "break";
<li>CONTINUE_STATEMENT = "continue";
<li>RETURN_STATEMENT = "return", [EXPRESSION];
<li>EXPRESSION = LOGICAL_TERM, { ("||"), LOGICAL_TERM };
<li>LOGICAL_TERM = LOGICAL_FACTOR, { ("&&"), LOGICAL_FACTOR };
<li>LOGICAL_FACTOR = ("!" | LOGICAL_PRIMARY) | RELATIONAL_EXPRESSION;
<li>RELATIONAL_EXPRESSION = CONCAT_EXPRESSION, { ("==" | "!=" | "<" | ">" | "<=" | ">="), CONCAT_EXPRESSION };
<li>CONCAT_EXPRESSION = ARITHMETIC_EXPRESSION, { (".."), ARITHMETIC_EXPRESSION };
<li>ARITHMETIC_EXPRESSION = TERM, { ("+" | "-"), TERM };
<li>TERM = FACTOR, { ("*" | "/" | "%"), FACTOR };
<li>FACTOR = ("+" | "-" | "!"), FACTOR | SIMPLE_EXPRESSION;
<li>SIMPLE_EXPRESSION = IDENTIFIER | STRING_LITERAL | NUMBER_LITERAL | "(", EXPRESSION, ")" | FUNCTION_CALL_STATEMENT | ARRAY_ACCESS | ENUM_ACCESS | MEMBER_ACCESS;
<li>IDENTIFIER = LETTER, { LETTER | DIGIT | "_" };
<li>STRING_LITERAL = '"', { LETTER | DIGIT | SPECIAL_CHARACTER }, '"';
<li>NUMBER_LITERAL = DIGIT, { DIGIT };
<li>ARRAY_ACCESS = IDENTIFIER, "[", EXPRESSION, "]";
<li>ENUM_ACCESS = IDENTIFIER, ":", IDENTIFIER;
<li>MEMBER_ACCESS = IDENTIFIER, ".", IDENTIFIER;
<li>TYPE = IDENTIFIER;
<li>LETTER = ("a" | "b" | "c" | "d" | "e" | "f" | "g" | "h" | "i" | "j" | "k" | "l" | "m" | "n" | "o" | "p" | "q" | "r" | "s" | "t" | "u" | "v" | "w" | "x" | "y" | "z" | "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" | "I" | "J" | "K" | "L" | "M" | "N" | "O" | "P" | "Q" | "R" | "S" | "T" | "U" | "V" | "W" | "X" | "Y" | "Z");
<li>DIGIT = ("0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9");

## Syntatic Diagram

Generated using DrawGrammar (https://jacquev6.github.io/DrawGrammar/)

![Syntactic Diagram](syntactic_diagram.png)

## Usage

To use the HRL language, you can write your code in a text editor and save it with a `.hr` extension. You can then use hrl-validator to check the syntax of your code and hrl-interpreter to run your code.

### Syntax Validation

```bash
cd hrl-validator
make
./hrl_validator < <path-to-your-hrl-file>
```

### Interpretation (prototype; behaviors and pub/sub system not implemented yet)

```bash
cd hrl-interpreter
make
./hrl_interpreter <path-to-your-hrl-file>
```

## Examples

You can find examples of HRL code in the `examples` directory.