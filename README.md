# HRL (Human-Robot Language)

HRL is a language prototype specifically designed to help developers and product owners program AI-powered systems for a wide range of applications, including chatbots, virtual assistants, and physical robots. The language is designed to be easy to learn and use, with a clean and concise syntax that allows for rapid integration and development.

## Key Features

- **Easy Parallelization**: HRL provides built-in constructs for parallel execution and load balancing, allowing developers to easily distribute tasks across multiple machines or processes.

- **Pub/Sub Topics**: The language supports a publish/subscribe model for communication between different components of a system, where modules or components can publish messages (events) to specific topics and subscribe to receive messages from topics of interest.

- **Context Switching Mechanism**: Components can easily adapt to different contexts or environments by switching between different sets of rules or behaviors based on the current context or state of the system.

- **Session Management**: Built-in support is provided for managing active user sessions and maintaining state across multiple interactions. Topics can be used to store and retrieve the associated data for each session, and components can be session-aware. This includes error handling, since errors can also be isolated to not affect the entire system.

- **Setup-Loop by Default**: Programs by default run in an infinite main loop after setup, allowing for continuous execution and monitoring of the system.

- **Easy Integration**: HRL code can easily call scripts written in Python, C++ and other languages and capture their output. This makes it very easy to integrate existing models and libraries, so you can focus on the high-level pipeline and logic.

## Example Code (under development)

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
        userInput := waitForUserInput(session.id)

        // Publish user input to topic
        publishToTopic(Topic.UserInput, userInput)

        // Process user input based on context
        switch userInput {
            case "start task":
                switchContext(session, Context.Task)
            case "error":
                switchContext(session, Context.ErrorHandling)
            default:
                respond(session.id, "Sorry, I didn't understand.")
        }
    }
}

behavior taskContext(session: Session) {
    loop {
        // Perform task-specific operations
        performTask(session.id)

        // Publish task completion to topic
        publishToTopic(Topic.TaskCompleted, session.id)

        // Listen for user input to switch context
        userInput := waitForUserInput(session.id)
        if userInput == "exit" {
            switchContext(session, Context.General)
        }
    }
}

behavior errorHandlingContext(session: Session) {
    loop {
        // Handle errors
        handleError(session.id)

        // Publish error to topic
        publishToTopic(Topic.ErrorOccurred, session.id)

        // Listen for user input to switch context
        userInput := waitForUserInput(session.id)
        if userInput == "retry" {
            switchContext(session, Context.General)
        }
    }
}

setup {
    // Start pub/sub system
    startPubSubSystem()

    // Create sessions for each chat
    sessions := createSessions()

    // Start context-specific behaviors for each session in parallel
    for session in sessions {
        start generalContext(session)
        start taskContext(session)
        start errorHandlingContext(session)
    }
}

main {
    // Any main loop tasks here
}

```