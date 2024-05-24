#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

// Define typedefs
typedef struct
{
    float value;
    bool isOperator;
    char operator_;
} Number;

typedef struct Node
{
    void* data;
    struct Node* next;
} Node;

typedef struct Stack
{
    Node* top;
} Stack;

typedef struct Queue
{
    Node* front;
    Node* rear;
} Queue;

// Define typedef functions
void initStack(Stack* stack)
{
    stack->top = NULL;
}

bool isEmptyStack(Stack* stack)
{
    return stack->top == NULL;
}

void pushStack(Stack* stack, void* data)
{
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = stack->top;
    stack->top = newNode;
}

void* popStack(Stack* stack)
{
    if (isEmptyStack(stack))
    {
        return NULL;
    }
    Node* temp = stack->top;
    void* data = temp->data;
    stack->top = stack->top->next;
    free(temp);
    return data;
}

void* peekStack(Stack* stack)
{
    if(isEmptyStack(stack))
    {
        return NULL;
    }
    return stack->top->data;
}

void initQueue(Queue* queue)
{
    queue->front=NULL;
    queue->rear=NULL;
}

bool isEmptyQueue(Queue* queue)
{
    return queue->front == NULL;
}

void enqueue(Queue* queue, void* data)
{
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = NULL;
    if (isEmptyQueue(queue))
    {
        queue->front = newNode;
        queue->rear = newNode;
    }
    else
    {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
}

void* dequeue(Queue* queue)
{
    if(isEmptyQueue(queue))
    {
        return NULL;
    }

    Node* temp = queue->front;
    void* data = temp->data;
    queue->front = queue->front->next;

    if (queue->front == NULL)
    {
        queue->rear = NULL;
    }

    free(temp);
    return data;
}

void freeQueue(Queue* queue)
{
    Node* temp;

    while(queue->front != NULL)
    {
        temp = queue->front;
        queue->front = queue->front->next;
        free(temp);
    }
}

int getPrecedence(char op)
{
    switch(op)
    {
        case '^':
            return 3;
        case '*':
        case '/':
            return 2;
        case '+':
        case '-':
            return 1;
        default:
            return 0;
    }
}


bool isoperator(char ch) {
  return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^';
}

void shuntingYard(const char* expression, Queue* output)
{
    // Declare and initialize operator stack
    Stack operatorStack;
    initStack(&operatorStack);

    // loop through expression char by char
    char ch;
    int i=0;

    while (expression[i] != '\0')
    {
        ch = expression[i];


        if (isdigit(ch) || ch == '.') //handle numbers
        {
            // initialize character array to hold number
            char number[100];
            int j=0;

            // fill array values with number
            while(isdigit(expression[i]) || expression[i] == '.')
            {
                number[j++] = expression[i++];
            }
            // end array with null character
            number[j] = '\0';

            // initialize a new Number object to add to queue
            Number* num = (Number*)malloc(sizeof(Number));
            num->value = atof(number);
            num->isOperator = false;

            // enqueue the number
            enqueue(output, num);

            // decrement i
            i--;
        }
        else if (ch == '(') // handle parenthesis
        {
            char* paren = (char*)malloc(sizeof(char));
            *paren = ch;
            pushStack(&operatorStack, (void*)paren); //push to stack
        }
        else if (ch == ')')
        {
            /* while the stack is not empty and the character at...
             the top is not '(' push everything to the operator queue.*/
            while(!isEmptyStack(&operatorStack) && *((char*)peekStack(&operatorStack)) != '(')
            {
                enqueue(output, popStack(&operatorStack));
            }
            // if the stack is empty exit early
            if (isEmptyStack(&operatorStack))
            {
                printf("Matching ( not found.\n");
                return;
            }
            // remove ( from the stack
            popStack(&operatorStack);
        }
        else if (isoperator(ch)) // handle operators
        {
            /* while the stack is not empty and the precedence of the...
               current top of the stack is greater than or equal to the...
               current character, enqueue everything from the stack onto output queue*/
            while(!isEmptyStack(&operatorStack) && getPrecedence(*(char*)peekStack(&operatorStack)) >= getPrecedence(ch))
            {
                enqueue(output, popStack(&operatorStack));
            }
            Number* op = (Number*)malloc(sizeof(Number));
            op->isOperator = true;
            op->operator_= ch;
            pushStack(&operatorStack, op);
        }
        i++;
    }

    while(!isEmptyStack(&operatorStack))
    {
        enqueue(output, popStack(&operatorStack));
    }
}

float performOperation(float x, float y, char operator_)
{
    //switch statement to handle operations
    switch(operator_)
    {
        case '+':
            return x+y;
        case '-':
            return x-y;
        case '*':
            return x*y;
        case '/':
            if (y == 0)
            {
                printf("Error: Division by zero.\n");
                return NAN;
            }
            return x/y;
        case '^':
            return pow(x, y);
        default:
            printf("Error: Invalid operation.\n");
            return NAN;
    }
}

float evaluatePostfix(Queue* postfix)
{
    // initialize operand stack
    Stack operandStack;
    initStack(&operandStack);

    // while the queue is not empty
    while (!isEmptyQueue(postfix))
    {
        // get current number in postfix queue
        Number* number = (Number*)dequeue(postfix);

        // handle nulls
        if (number == NULL)
        {
            continue;
        }

        if (!number->isOperator)
        {
            pushStack(&operandStack, number);
        }
        else
        {
            if(isEmptyStack(&operandStack) || operandStack.top->next == NULL)
            {
                printf("Error: Insufficient operands for operator.\n");
                return NAN;
            }
            float y = ((Number*)popStack(&operandStack))->value;
            float x = ((Number*)popStack(&operandStack))->value;
            float result = performOperation(x, y, number->operator_);
            Number* resNum = (Number*)malloc(sizeof(Number));
            resNum->value = result;
            resNum->isOperator = false;
            pushStack(&operandStack, resNum);
            free(number);
        }
    }

    if(operandStack.top->next == NULL)
    {
        return *(float*)popStack(&operandStack);
    }
    else
    {
        printf("Error: Extra elements in postfix notation.\n");
        return NAN;
    }
}

int main()
{
    // initialize expression
    char expression[100];
    char choice;

    do
    {
         // ask user for input
        system("cls");
        printf(R"(
         _____      _            _       _
        / ____|    | |          | |     | |
       | |     __ _| | ___ _   _| | __ _| |_ ___  _ __
       | |    / _` | |/ __| | | | |/ _` | __/ _ \| '__|
       | |___| (_| | | (__| |_| | | (_| | || (_) | |
        \_____\__,_|_|\___|\__,_|_|\__,_|\__\___/|_|
)");
        printf("\n\n\tUses the shunting yard algorithm to calculate what you want!");
        printf("\n\n\tEnter your expression: \t");
        fgets(expression, sizeof(expression), stdin);

        // remove newline character from end
        expression[strcspn(expression, "\n")] = '\0';

        // initialize prefix and postfix queues
        Queue postfixQueue;
        initQueue(&postfixQueue);


        // use shunting yard algorithm
        shuntingYard(expression, &postfixQueue);

        // get result
        float result = evaluatePostfix(&postfixQueue);

        system("cls");
        // check result
        if(isnan(result))
        {
            printf("Error: Invalid Expression.\n");
        }
        else
        {
            printf(R"(
     __     __                _____                 _ _
     \ \   / /               |  __ \               | | |
      \ \_/ /__  _   _ _ __  | |__) |___  ___ _   _| | |_
       \   / _ \| | | | '__| |  _  // _ \/ __| | | | | __|
        | | (_) | |_| | |    | | \ \  __/\__ \ |_| | | |_
        |_|\___/ \__,_|_|    |_|  \_\___||___/\__,_|_|\__|


)");

            printf("\n\n\t%s = %g\n\n\t", expression, result);
        }

        // free memory from queues
        freeQueue(&postfixQueue);

        printf("Calculate another expression? (y/n): ");
        fgets(expression, sizeof(expression), stdin); // get input buffer ready for next expression
        sscanf(expression, "\t\t%c", &choice);

    } while (choice == 'y' || choice == 'Y');


    return 0;

}
