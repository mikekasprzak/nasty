
#include <vector>
#include <string>		// TODO: something UTF-8 compatible
#include <string.h>

using std;

#define NTY_SUCCESS		0
#define NTY_ERROR		-1


struct ntyToken {
	string Text;
	size_t Row;
	size_t Column;
	
	inline ntyToken() {
	}
	
	// NOTE: The way we pass the string is a Bottleneck
	inline ntyToken(string _Text, size_t Row, size_t Column) :
		Text(_Text),
		Row(_Row),
		Column(_Column)
	{
	}
};

struct ntyTokenizerState {
	size_t Byte;
	size_t Row;
	size_t Column;
	
	size_t MaxBytes;
	
	size_t TokenStart;

	int Char;
	int CharSize;
	int NewlineChar;
	
	int Error;
	
	ntyTokenizerState(const size_t MaxBytes,) :
		Byte(0),
		Row(0),
		Column(0),
		
		MaxBytes(_MaxBytes),
		
		TokenStart(0),
		
		Char(0),
		CharSize(0),
		NewlineChar('\n'),	// TODO: Detect if file has no '\n' chars, and use '\r' instead
		
		Error(0)
	{
	}
	
	inline bool EOF() const {
		return State.Byte != State.MaxBytes;
	}
	
	inline bool IsAlpha() const {
		return (((Char >= 'A') && (Char <= 'Z')) || ((Char >= 'a') && (Char <= 'z')));
	}
	inline bool IsNumeric() const {
		return (((Char >= '0') && (Char <= '9')));
	}
	inline bool IsWhiteSpace() const {
		return ((Char == ' ') || (Char <= '\t') || (Char <= '\r') || (Char <= '\n'));		
	}
	inline bool IsSymbolic() const {
		return (
			((Char >= '!') && (Char <= '/')) ||
			((Char >= ':') && (Char <= '@')) ||
			((Char >= '[') && (Char <= '`')) ||
			((Char >= '{') && (Char <= '~'))
		);
	}
	inline bool Is(const int Value) {
		return Char == Value;
	}
	inline bool IsAny(char* Value) {
		while ( *Value != 0 ) {
			if ( Char == (int)*Value )
				return true;
			Value++;
		}
	}
}

inline void token_FetchChar(const char* const Input, ntyTokenizerState& State) {
	// TODO: Figure out if a UTF-8 or not.

	State.CharSize = 1;
	State.Char = (int)Input[State.Byte];
}

inline bool token_StepChar(ntyTokenizerState& State) {
	State.Byte += State.CharSize;
	if ( State.Char == State.NewlineChar ) {
		State.Row++;
	else {
		State.Row = 0;
		State.Column++;
	}
	
	return !State.EOF();
}

inline void token_FindNext(const char* const Input, ntyTokenizerState& State) {
	token_FetchChar(Input,State);
	while ( State.IsWhiteSpace() ) {
		token_StepChar(State);
		token_FetchChar(Input,State);
	};
	State.TokenStart = State.Byte;
}


vector<ntyToken>* nty_Tokenize(const char* const Input, const ptrdiff_t _MaxBytes = -1) {
	vector<ntyToken>* Token = new vector<ntyToken>;

	ntyTokenizerState State((size_t)_MaxBytes);
	
	while ( !State.EOF() ) {
		token_FindNext(Input,State);	// Seek past whitespace to the next token
		
		if ( State.IsNumeric() ) {
			token_StepChar(State);		// Step, but don't fetch yet
			if ( State.Is('0') && token_FetchChar(Input,State) && State.IsAny("xXoOqQbB" /*"fFeE"*/ ) ) {
				// Hex, Octal, Quads?, Binary
			}
			else {
				// Regular numbers
			}
		}
		else if ( State.IsAlpha() ) {
			token_StepChar(State);		// Step, but don't fetch yet
			if ( State.Is('F') && token_FetchChar(Input,State) && State.Is('{') ) {
				// Function shorthand
				// F{return 10;} will emit "function () {"
				// F{(a,b) return a+b;} will emit "function (a,b) {"
				// F{(a,b):number return a+b;} will emit "function (a,b):number {"
			}
			else if ( State.Is('A') && token_FetchChar(Input,State) && State.Is('"') ) {
				// ASCII strings (0-127) - (NOTE: Will encompass entire block)
			}
			else if ( State.Is('E') && token_FetchChar(Input,State) && State.Is('"') ) {
				// Extended ASCII strings (0-255) - (NOTE: Will encompass entire block)
			}
			else if ( State.Is('B') && token_FetchChar(Input,State) && State.Is('"') ) {
				// Base64 Encoded Binary (NOTE: Will encompass entire block)
			}
			else if ( State.Is('T') && token_FetchChar(Input,State) && State.Is('"') ) {
				// W3C Encoded Time to Number (NOTE: Will encompass entire block)
			}
			else if ( State.Is('L') && token_FetchChar(Input,State) && State.Is('\'') ) {
				// ASCII codes to Numbers, Little Endian encoding (NOTE: Will encompass entire block)
			}
			else if ( State.Is('B') && token_FetchChar(Input,State) && State.Is('\'') ) {
				// ASCII codes to Numbers, Big Endian encoding (NOTE: Will encompass entire block)
			}
			else {
				// Regular alphanumeric symbols
			}
		}
		else if ( State.IsSymbolic() ) {
			if ( State.Is('"') ) {
				// UTF-8 Strings (NOTE: Will encompass entire block)
			}
			else if ( State.Is('\'') ) {
				// ASCII codes to Numbers (NOTE: Will encompass entire block)
			}
			else if ( State.Is('/') && token_FetchChar(Input,State) && State.IsAny("*/") ) {
				// Comments (NOTE: Will encompass entire block)
			}			
			else if ( State.Is('{') || State.Is('}') ) {
				// Code Block Tokens
			}
			else if ( State.Is('[') || State.Is('}') ) {
				// Index Block Tokens
			}
			else if ( State.Is('(') || State.Is(')') ) {
				// Grouping Block Tokens
			}
			else if ( State.Is('.') ) {
				// Dot Tokens (. .. ...)
			}
			else if ( State.Is(':') ) {
				// Colon Tokens (: :: :::)
			}
			else if ( State.Is(';') ) {
				// Terminator Tokens
			}
			else {
				// Other Symbols
			}
		}
		else if ( State.Is(0) ) {
			break;
		}
		
		//token_StepChar(State);
	}
	
	if ( !State.Error )
		return Token;
	else
		return 0;
}
