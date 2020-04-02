
#include <iostream>
#include <string>
#include <queue>
#include <map>

enum parserState
{
  emptyToken,
  midToken,
  completedToken
};

enum mathNodeType
{
  none = -1,
  /* single char symbols are encoded by their ascii representation */
  /* *, /, ^, (), +, - */
  literal = 255 /* integer value for now */
};

std::map<int, int> opPriorityMap;

struct mathNodeToken
{
  mathNodeType nodeType;
  std::string data;
};

struct expressionTree
{
  expressionTree *left;
  expressionTree *right;
  mathNodeToken token;
};

auto getExpressionTree(std::queue<mathNodeToken> &tokens) -> expressionTree *;
auto getSubexpression(std::queue<mathNodeToken> &tokens, int prevOpPriority) -> expressionTree *;
auto evaluateExpressionTree(expressionTree *root) -> int;

int evaluateExpression(std::string s)
{
  int parseCursorIndex = 0;
  std::queue<mathNodeToken> tokenQueue;
  int result = 0;

  int beginChar = -1;
  int endChar = -1;
  mathNodeType nodeType;
  parserState currentState = parserState::emptyToken;

  bool error = false;
  while (parseCursorIndex < s.length() && !error)
  {
    char c = s[parseCursorIndex];
    switch (currentState)
    {
    case parserState::emptyToken:
    {
      if (isspace(c))
      {
        parseCursorIndex++;
        break;
      }

      beginChar = parseCursorIndex;
      switch (c)
      {
      case '+':
      case '-':
      case '*':
      case '/':
        currentState = parserState::completedToken;
        endChar = parseCursorIndex;
        nodeType = static_cast<mathNodeType>(c);
        break;
      default:
        if (c >= '0' && c <= '9')
        {
          currentState = parserState::midToken;
          beginChar = parseCursorIndex;
        }
        else
        {
          std::cout << "Unexpected character encountered at position " << parseCursorIndex;
          error = true;
        }
        break;
      }
      break;
    }
    case parserState::midToken:
    {

      if (c >= '0' && c <= '9')
      {
        char peakChar = s[parseCursorIndex + 1];
        if (peakChar < '0' || peakChar > '9')
        {
          if (isspace(peakChar) ||
              peakChar == '\0' ||
              peakChar == '+' ||
              peakChar == '-' ||
              peakChar == '*' ||
              peakChar == '/')
          {
            currentState = parserState::completedToken;
            endChar = parseCursorIndex;
            nodeType = mathNodeType::literal;
          }
          else
          {
            std::cout << "Unexpected character encountered at position " << parseCursorIndex;
            error = true;
          }
        }
        else
        {
          parseCursorIndex++;
        }
      }
      break;
    }
    case parserState::completedToken:
    {
      mathNodeToken token;
      token.nodeType = nodeType;
      token.data = s.substr(beginChar, endChar - beginChar + 1);
      tokenQueue.push(token);
      parseCursorIndex++;

      currentState = parserState::emptyToken;
      beginChar = -1;
      endChar = -1;
      break;
    }
    default:
      break;
    }
  }

  expressionTree *root = getExpressionTree(tokenQueue);
  result = evaluateExpressionTree(root);
  return result;
}

auto getExpressionTree(std::queue<mathNodeToken> &tokens) -> expressionTree *
{
  expressionTree *root = new expressionTree();
  root->token = tokens.front();
  tokens.pop();

  while (!tokens.empty())
  {
    expressionTree *op = new expressionTree();
    op->token = tokens.front();
    tokens.pop();

    op->left = root;
    root = op;

    root->right = getSubexpression(tokens, opPriorityMap[root->token.nodeType]);
  }

  return root;
}

auto getSubexpression(std::queue<mathNodeToken> &tokens, int prevOpPriority) -> expressionTree *
{
  expressionTree *left = new expressionTree();
  left->token = tokens.front();
  tokens.pop();

  if (!tokens.empty())
  {
    int currentOpPriority = opPriorityMap[tokens.front().nodeType];
    if (currentOpPriority >= prevOpPriority)
    {
      expressionTree *root = new expressionTree();
      root->token = tokens.front();
      tokens.pop();

      root->left = left;
      root->right = getSubexpression(tokens, currentOpPriority);
      return root;
    }
    else
    {
      return left;
    }
  }
  else
  {
    return left;
  }
}

auto evaluateExpressionTree(expressionTree *root) -> int
{
  if (root->token.nodeType == mathNodeType::literal)
  {
    // std::cout << root->token.data << std::endl;
    return std::stoi(root->token.data);
  }
  else
  {
    int leftVal = evaluateExpressionTree(root->left);
    int rightVal = evaluateExpressionTree(root->right);
    switch (root->token.data[0])
    {
    case '+':
      return leftVal + rightVal;
    case '-':
      return leftVal - rightVal;
    case '*':
      return leftVal * rightVal;
    case '/':
      return leftVal / rightVal;
    default:
      break;
    }
  }
}

int main()
{
  opPriorityMap['+'] = 0;
  opPriorityMap['-'] = 0;
  opPriorityMap['*'] = 50;
  opPriorityMap['/'] = 50;

  std::string s;

  std::cout << "This is a simple math interpreter that evaluates a static non-algebraic expression..." << std::endl;
  std::getline(std::cin, s);

  std::cout << " = " << evaluateExpression(s) << std::endl;
}
