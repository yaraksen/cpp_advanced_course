#include "tokenizer.h"
#include "error.h"

bool SymbolToken::operator==(const SymbolToken& other) const {
    return this->name == other.name;
}
SymbolToken::SymbolToken(std::string name) : name(name) {
}

bool QuoteToken::operator==(const QuoteToken&) const {
    return true;
}
bool DotToken::operator==(const DotToken&) const {
    return true;
}
bool ConstantToken::operator==(const ConstantToken& other) const {
    return this->value == other.value;
}
ConstantToken::ConstantToken(int value) : value(value) {
}

// Help functions -->
bool IsDigit(char token) {
    return 48 <= token && token <= 57;
}

bool IsSymbolStart(char token) {
    // [a-zA-Z<=>*#]
    return (97 <= token && token <= 122) || (65 <= token && token <= 90) || token == '+' ||
           token == '-' || token == '<' || token == '=' || token == '>' || token == '*' ||
           token == '#';
}

bool IsSymbol(char token) {
    // [a-zA-Z<=>*#0-9?!-]
    return IsSymbolStart(token) || IsDigit(token) || token == '?' || token == '!' || token == '-';
}

// Tokenizer -->
Tokenizer::Tokenizer(std::istream* in) : in_(in), token_(DotToken()) {
    Next();
}

bool Tokenizer::IsEnd() {
    return this->is_end_;
}

void Tokenizer::CheckNext() {
    if (!this->IsEnd()) {
        if (in_->peek() == std::char_traits<char>::eof()) {
            is_end_ = true;
        }
    }
}

void Tokenizer::Next() {
    CheckNext();
    if (!this->IsEnd()) {
        char token = in_->get();
        if (token == '\'' || token == '"') {
            token_ = QuoteToken();
        } else if (token == '.') {
            token_ = DotToken();
        } else if ((token == '-' && IsDigit(in_->peek())) || IsDigit(token)) {
            std::string constant{token};
            while (in_->peek() != std::char_traits<char>::eof() && IsDigit(in_->peek())) {
                constant.push_back(in_->get());
            }
            token_ = ConstantToken(std::stoi(constant));
        } else if (token == '(') {
            token_ = BracketToken::OPEN;
        } else if (token == ')') {
            token_ = BracketToken::CLOSE;
        } else if (IsSymbolStart(token)) {
            std::string constant{token};
            while (in_->peek() != std::char_traits<char>::eof() && IsSymbol(in_->peek())) {
                constant.push_back(in_->get());
            }
            token_ = SymbolToken(constant);
        } else if (token == ' ' || token == '\n') {
            Next();
        } else {
            throw SyntaxError("Incorrect symbol");
        }
    }
}

Token Tokenizer::GetToken() {
    return token_;
}