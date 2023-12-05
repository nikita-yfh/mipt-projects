#ifndef SYNTAXERR_H
#define SYNTAXERR_H

struct FilePosition {
	int line;
	int column;
};

struct SyntaxError {
	struct FilePosition position;
	int type;
};

#endif // SYNTAXERR_H
