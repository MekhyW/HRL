# HRL (Human-Robot Language)

HRL is a language prototype specifically designed to help developers and product owners program AI-powered systems for a wide range of applications, including chatbots, virtual assistants, and physical robots. The language is designed to be easy to learn and use, with a clean and concise syntax that allows for rapid integration and development. Programming in HRL can be viewed as being conceptually similar to creating a finite state machine (FSM), especially when dealing with context management and state transitions.

## Key Features

- **Easy Parallelization**: HRL provides built-in constructs for parallel execution and load balancing, allowing developers to easily distribute tasks across multiple machines or processes.

- **Pub/Sub Topics**: The language supports a publish/subscribe model for communication between different components of a system, where modules or components can publish messages (events) to specific topics and subscribe to receive messages from topics of interest.

- **Context Switching Mechanism**: Components can easily adapt to different contexts or environments by switching between different sets of rules or behaviors based on the current context or state of the system.

- **Session Management**: Built-in support is provided for managing active user sessions and maintaining state across multiple interactions. Topics can be used to store and retrieve the associated data for each session, and components can be session-aware. This includes error handling, since errors can also be isolated to not affect the entire system.

- **Setup-Loop by Default**: Programs by default run in an infinite main loop after setup, allowing for continuous execution and monitoring of the system.

- **Easy Integration**: HRL code can easily call scripts written in Python, C++ and other languages and capture their output. This makes it very easy to integrate existing models and libraries for common agent tasks (e.g., natural language processing, vision models, audio models, path planning, etc.), so you can focus on the higher-level logic.

## Example Code

```hrl
// Define context IDs
enum Context {
    General,
    Task,
    ErrorHandling
}

// Define session data structure
struct Session {
    id: String,
    context: Context,
    // Other session data
}

// Define pub/sub topics
enum Topic {
    UserInput,
    TaskCompleted,
    ErrorOccurred
}

// Context-specific behaviors
behavior generalContext(session: Session) {
    loop {
        // Listen for user input
        const userInput = waitForUserInput(session.id);

        // Publish user input to topic
        publishToTopic(Topic.UserInput, userInput);

        // Process user input based on context
        if (userInput == "start task") {
            switchContext(session, Context.Task);
        } else if (userInput == "error") {
            switchContext(session, Context.ErrorHandling);
        } else {
            respond(session.id, "Sorry, I didn't understand.");
        }
    }
}

behavior taskContext(session: Session) {
    loop {
        // Perform task-specific operations
        performTask(session.id);

        // Publish task completion to topic
        publishToTopic(Topic.TaskCompleted, session.id);

        // Listen for user input to switch context
        const userInput = waitForUserInput(session.id);
        if (userInput == "exit") {
            switchContext(session, Context.General);
        }
    }
}

behavior errorHandlingContext(session: Session) {
    loop {
        // Handle errors
        handleError(session.id);

        // Publish error to topic
        publishToTopic(Topic.ErrorOccurred, session.id);

        // Listen for user input to switch context
        const userInput = waitForUserInput(session.id);
        if (userInput == "retry") {
            switchContext(session, Context.General);
        }
    }
}

setup {
    // Start pub/sub system
    startPubSubSystem();

    // Create sessions for each chat
    const sessions = createSessions();

    // Start context-specific behaviors for each session in parallel
    loop {
        session = sessions.pop();
        if (session.context == Context.General) {
            startBehavior(generalContext, session);
        } else if (session.context == Context.Task) {
            startBehavior(taskContext, session);
        } else if (session.context == Context.ErrorHandling) {
            startBehavior(errorHandlingContext, session);
        }
        if (sessions.isEmpty()) {
            break;
        }
    }
}

main {
    // Any main loop tasks here
}
```

## Integrations

The following subscript demonstrates how the language can be used to integrate different services and other scripts. 

In this example, the language is used to create a simple chatbot that can detect objects in images and generate descriptions for them using AI models and text-to-speech services, by calling OpenCV, MetaAISAM, Google Cloud Text-to-Speech, and a custom Python script for image capture (whose terminal output is captured and returned as a string).

No import statements are needed, as the language is designed to be able to call external scripts and services directly, locating the dependencies automatically.

```hrl
behavior objectDetectionContext(session: Session) {
    loop {
        image = opencv.captureImage(session.id);
        objects = metaAISAM.segment(image);
        publishToTopic(Topic.ObjectDetected, objects);
        switchContext(session, Context.Description);
    }
}

behavior descriptionContext(session: Session) {
    loop {
        objects = waitForMessage(Topic.ObjectDetected);
        description = generateDescription(objects);
        googleCloudTTS.speak(session.id, description);
        publishToTopic(Topic.DescriptionCompleted, session.id);
        switchContext(session, Context.Idle);
    }
}

behavior idleContext(session: Session) {
    loop {
        trigger = waitForImageCaptureTrigger(session.id);
        if (trigger == "capture") {
            switchContext(session, Context.ObjectDetection);
        }
    }
}

function generateDescription(objects: List<Object>): String {
    description = "Detected objects: ";
    loop {
        object = objects.pop();
        description += object.name + ", ";
        if (objects.isEmpty()) {
            break;
        }
    }
    return description.trim();
}

function waitForImageCaptureTrigger(sessionId: String): String {
    return python.runScript("image_capture.py", sessionId);
}
```

## EBNF (Extended Backus-Naur Form) Grammar

<li>PROGRAM = SETUP, MAIN;
<li>SETUP = "setup", BLOCK;
<li>MAIN = "main", BLOCK;
<li>STATEMENTS = (VARIABLE_DECLARATION | ENUM_DECLARATION | STRUCT_DECLARATION | BEHAVIOR_DECLARATION | LOOP_STATEMENT | IF_STATEMENT | SWITCH_STATEMENT | FUNCTION_CALL_STATEMENT | FUNCTION_DECLARATION | CONST_DECLARATION | THREADLOOP_STATEMENT | BREAK_STATEMENT | CONTINUE_STATEMENT | RETURN_STATEMENT), ";";
<li>VARIABLE_DECLARATION = ("const" | lambda), IDENTIFIER, ":", TYPE, "=", EXPRESSION;
<li>ENUM_DECLARATION = "enum", IDENTIFIER, "{", ENUM_MEMBER, { ",", ENUM_MEMBER }, "}";
<li>ENUM_MEMBER = IDENTIFIER;
<li>STRUCT_DECLARATION = "struct", IDENTIFIER, "{", VARIABLE_DECLARATION, { ",", VARIABLE_DECLARATION }, "}";
<li>BEHAVIOR_DECLARATION = "behavior", IDENTIFIER, "(", PARAMETERS, ")", BLOCK;
<li>PARAMETERS = (PARAMETER, { ",", PARAMETER });
<li>PARAMETER = IDENTIFIER, ":", TYPE;
<li>BLOCK = "{", {STATEMENTS}, "}";
<li>LOOP_STATEMENT = "loop", BLOCK;
<li>IF_STATEMENT = "if", "(", EXPRESSION, ")", BLOCK, ["else", BLOCK];
<li>SWITCH_STATEMENT = "switch", EXPRESSION, "{", CASE_BLOCK, { CASE_BLOCK }, "}";
<li>CASE_BLOCK = "case", EXPRESSION, ":", STATEMENTS;
<li>FUNCTION_CALL_STATEMENT = IDENTIFIER, "(", [ARGUMENTS], ")";
<li>ARGUMENTS = [EXPRESSION, { ",", EXPRESSION }];
<li>FUNCTION_DECLARATION = "function", IDENTIFIER, "(", PARAMETERS, ")", ":", TYPE, BLOCK;
<li>CONST_DECLARATION = "const", IDENTIFIER, "=", EXPRESSION;
<li>THREADLOOP_STATEMENT = "threadloop", IDENTIFIER, "(", ARGUMENTS, ")";
<li>BREAK_STATEMENT = "break";
<li>CONTINUE_STATEMENT = "continue";
<li>RETURN_STATEMENT = "return", [EXPRESSION];
<li>EXPRESSION = SIMPLE_EXPRESSION, { RELATIONAL_OPERATOR, SIMPLE_EXPRESSION };
<li>RELATIONAL_OPERATOR = "==", "!=";
<li>SIMPLE_EXPRESSION = IDENTIFIER | STRING_LITERAL | NUMBER_LITERAL | "(", EXPRESSION, ")" | FUNCTION_CALL_STATEMENT | ARRAY_ACCESS | MEMBER_ACCESS;
<li>IDENTIFIER = LETTER, { LETTER | DIGIT | "_" };
<li>STRING_LITERAL = '"', { LETTER | DIGIT | SPECIAL_CHARACTER }, '"';
<li>NUMBER_LITERAL = DIGIT, {DIGIT};
<li>ARRAY_ACCESS = IDENTIFIER, "[", EXPRESSION, "]";
<li>MEMBER_ACCESS = IDENTIFIER, ".", IDENTIFIER;
<li>TYPE = IDENTIFIER;
<li>LETTER = ("a" | "b" | "c" | "d" | "e" | "f" | "g" | "h" | "i" | "j" | "k" | "l" | "m" | "n" | "o" | "p" | "q" | "r" | "s" | "t" | "u" | "v" | "w" | "x" | "y" | "z" | "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" | "I" | "J" | "K" | "L" | "M" | "N" | "O" | "P" | "Q" | "R" | "S" | "T" | "U" | "V" | "W" | "X" | "Y" | "Z");
<li>DIGIT = ("0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9");

## Syntatic Diagram

Generated using DrawGrammar (https://jacquev6.github.io/DrawGrammar/)

![Syntactic Diagram](syntactic_diagram.png)