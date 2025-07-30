#include <iostream>
#include <stack>
#include <cctype>
#include <map>
#include <string>
#include <cmath>
#include <sstream>
using namespace std;

// Converts infix to postfix
class Expression {
    string infix;
    string postfix;

    // Returns precedence of operators
    int GetOperatorPrecedence(char o) {
        if(o == '+' || o == '-') {
            return 1;
        }
        else if(o == '*' || o == '/') {
            return 2;
        }
        else if(o == '^') {
            return 3;
        }
        else if(o == '(' || o == ')') {
            return 4;
        }
        return 0;
    }

    // Checks if character is an operator
    bool IsOperator(char o) {
        if(o == '+' || o == '-' || o == '*' || o == '/' || o == '^' || o == '(' || o == ')') {
            return true;
        }
        return false;
    }

    // Converts infix string to postfix string
    void ConvertToPostfix(string infix) {
        string postfix_result;
        stack<string> st;

        for (int i = 0; i < infix.length(); i++) {
            char c = infix[i];

            // Handle numbers
            if (isdigit(c)) {
                string number;
                while (i < infix.length() && isdigit(infix[i])) {
                    number += infix[i];
                    i++;
                }
                i--;
                postfix_result += number + " ";
            }
            // Handle variables
            else if (isalpha(c)) {
                postfix_result += c;
                postfix_result += " ";
            }
            // Handle operators
            else if (IsOperator(c)) {
                string current_top = "";
                if (!st.empty()) {
                    current_top = st.top();
                }
                if (st.empty() || current_top == "(") {
                    st.push(string(1, c));
                }
                else if (c == '(') {
                    st.push(string(1, c));
                }
                else if (c == ')') {
                    while (!st.empty() && st.top() != "(") {
                        postfix_result += st.top() + " ";
                        st.pop();
                    }
                    if (!st.empty() && st.top() == "(") {
                        st.pop();
                    }
                }
                else if (GetOperatorPrecedence(c) > GetOperatorPrecedence(current_top[0])) {
                    st.push(string(1, c));
                }
                else if (GetOperatorPrecedence(c) == GetOperatorPrecedence(current_top[0])) {
                    postfix_result += st.top() + " ";
                    st.pop();
                    st.push(string(1, c));
                }
                else if (GetOperatorPrecedence(c) < GetOperatorPrecedence(current_top[0])) {
                    while (!st.empty() && GetOperatorPrecedence(c) < GetOperatorPrecedence(st.top()[0])) {
                        postfix_result += st.top() + " ";
                        st.pop();
                    }
                    st.push(string(1, c));
                }
            }
        }
        // Pop remaining operators
        while (!st.empty()) {
            postfix_result += st.top() + " ";
            st.pop();
        }
        postfix = postfix_result;
    }

public:
    // Constructor
    Expression(string infix) {
        SetExpression(infix);
    }

    // Sets the infix and computes postfix
    void SetExpression(string infix) {
        this->infix = infix;
        ConvertToPostfix(infix);
    }

    // Returns infix string
    string GetInfix() {
        return infix;
    }

    // Returns postfix string
    string GetPostfix() {
        return postfix;
    }
};

// Node for expression tree
class ExpressionNode {
    public:
        string token; // value or operator
        float result;
        ExpressionNode* leftChild;
        ExpressionNode* rightChild;

        // Constructor
        ExpressionNode(string t) {
            this->token = t;
            leftChild = nullptr;
            rightChild = nullptr;
        }
};

// Builds and evaluates expression tree
class ExpressionTree {
    private:
        string infix;
        string postfix;
        ExpressionNode* root;

        // Checks if character is an operator
        bool IsOperator(char o) {
            if(o == '+' || o == '-' || o == '*' || o == '/' || o == '^' || o == '(' || o == ')') {
                return true;
            }
            return false;
        }

        // Builds the tree from postfix
        void BuildExpression() {
            stack<ExpressionNode*> st;
            string token;
            istringstream iss(postfix);

            while (iss >> token) {
                // If token is number or variable
                if (isdigit(token[0]) || (token.size() > 1 && isdigit(token[1])) || isalpha(token[0])) {
                    ExpressionNode* newNode = new ExpressionNode(token);
                    st.push(newNode);
                } 
                // If token is operator
                else if (IsOperator(token[0])) {
                    ExpressionNode* right = st.top(); st.pop();
                    ExpressionNode* left = st.top(); st.pop();
                    ExpressionNode* opNode = new ExpressionNode(token);
                    opNode->rightChild = right;
                    opNode->leftChild = left;
                    st.push(opNode);
                }
            }
            if (!st.empty()) {
                root = st.top();
            }
        }

        // Deletes the tree
        void ClearTree() {
            ClearTreeHelper(root);
        }

        // Helper to delete nodes
        void ClearTreeHelper(ExpressionNode* &node) {
            if(node == nullptr) {
                return;
            }
            ClearTreeHelper(node->leftChild);
            ClearTreeHelper(node->rightChild);
            delete node;
            node = nullptr;
        }

        // Recursively evaluates the tree
        float EvaluateHelper(ExpressionNode* node, map<char, float>& letters) {
            // If variable
            if(isalpha(node->token[0])) {
                return letters[node->token[0]];
            }
            // If number
            if(isdigit(node->token[0])) {
                return stof(node->token);
            }
            // Otherwise, operator
            float left = EvaluateHelper(node->leftChild, letters);
            float right = EvaluateHelper(node->rightChild, letters);
            if (node->token == "+") {
                return left + right;
            }
            else if (node->token == "-") {
                return left - right;
            }
            else if (node->token == "*") {
                return left * right;
            }
            else if (node->token == "/") {
                return left / right;
            }
            else if (node->token == "^") {
                return pow(left, right);
            }
            else {
                return 0;
            }
        }

    public:    
        // Constructor
        ExpressionTree() {
            root = nullptr;
            infix = "";
            postfix = "";
        }

        // Sets the infix expression and builds tree
        void SetExpression(string exp) {
            infix = exp;
            Expression e(infix);
            postfix = e.GetPostfix();
            ClearTree();
            BuildExpression();
        }

        // Evaluates the expression tree
        float EvaluateExpression(map<char, float> letters) {
            return EvaluateHelper(root, letters);
        }

        // Returns postfix string
        string GetPostfix() {
            return postfix;
        }

        // Returns infix string
        string GetInfix() {
            return infix;
        }
};