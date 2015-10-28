#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define byte unsigned char
#define bool byte
#define true 1
#define false 0

/*
	This is entirely temporary code written for speed. It should be re-written
	with an actual lexer at some point so it can become much more powerful.
	
	It is horrible and hacky. Currently it is not freeing any memory what so 
	ever for speed. All cleanup happens when application closes. 
	Such memory leak.
	
	Might add template generation to the pre-processor.
*/

/*
	This should ignore comments (Might cause some bugs in its current state.
*/

typedef struct String
{
	char* data;
	int length, readPosition;
} String;

void logString(String string)
{
	printf("%s\n", string.data);
}

String createString(char* data)
{
	String string = {0};
	string.length = strlen(data);
	string.readPosition = 0;
	string.data = malloc(string.length+1);
	memcpy(string.data, data, string.length);
	string.data[string.length] = '\0';
	return string;
}

void destroyString(String* string)
{
	free(string->data);
}

void seekToStringStart(String* string)
{
	string->readPosition = 0;
}

bool isWhiteSpace(char character)
{
	return (character == ' '
		|| character == ';'
		|| character == '\n'
		|| character == '\r'
		|| character == '\t');
}

#define BUFFER_LENGTH 512
String getNextFunction(String* string)
{
	int bracketDepth = 0;
	
	char buffer[BUFFER_LENGTH];
	int bufferPosition = 0;
	
	while(isWhiteSpace(string->data[string->readPosition])
		&& string->readPosition < string->length)
		++string->readPosition;
	
	while(string->readPosition < string->length
		&& bufferPosition < BUFFER_LENGTH-1
		&& (!isWhiteSpace(string->data[string->readPosition]) 
			|| bracketDepth > 0))
	{
		char c = string->data[string->readPosition];
		
		if(c == '(')
			++bracketDepth;
		if(c == ')')
			--bracketDepth;
			
		buffer[bufferPosition] = c;
		++string->readPosition;
		++bufferPosition;
	}
	
	buffer[bufferPosition] = '\0';
	
	return createString(buffer);
}

String loadFile(char* filePath)
{
	String output = {0};
	
	FILE* file = fopen(filePath, "rb");
	if(file)
	{
		fseek(file, 0, SEEK_END);
		output.length = ftell(file);
		fseek(file, 0, SEEK_SET);
		
		output.data = malloc(output.length+1);
		fread(output.data, 1, output.length, file);
		fclose(file);
		output.data[output.length] = '\0';
	}
	else 
		printf("Warning: \"%s\" does not exist!", filePath);
		
	return output;
}

bool stringBeginsWith(String string, String beginsWith)
{
	for(int i = 0; i < beginsWith.length; ++i)
	{
		if(string.length < i)
			return false;
			
		if(string.data[i] != beginsWith.data[i])
			return false;
	}
	
	return true;
}

typedef struct BehaviorInfo
{
	char* name,
		* structName, 
		* updateProcName, 
		* drawProcName;
} BehaviorInfo;

char* cloneCString(char* string)
{
	char* clone = (char*)malloc(strlen(string)+1);
	strcpy(clone, string);
	return clone;
}

#define MAX_BEHAVIORS 5000

int main(int argc, char** argv)
{
	String defineBehavior = createString("defineBehavior");
	
	BehaviorInfo* behaviors = malloc(sizeof(BehaviorInfo) * MAX_BEHAVIORS);
	int behaviorCount = 0;
	
	for(int i = 1; i < argc; ++i)
	{
		//printf("%d: %s\n", i, argv[i]);
		
		String fileString = loadFile(argv[i]);

		while(fileString.readPosition != fileString.length)
		{
			String word = getNextFunction(&fileString);
			if(stringBeginsWith(word, defineBehavior))
			{
				// logString(word);
				
				// Read the arguments of the function into a behavior info
				BehaviorInfo* b = &behaviors[behaviorCount++];
				char* test = "test";
				b->name = test;
				
				bool readingArgs = false;
				int argCount = 0;
				
				char buffer[200];
				int bufferPosition = 0;
				
				while(word.readPosition < word.length)
				{
					char c = word.data[word.readPosition++];
					if(c == ' ')
						continue;
					else if(c == '(')
						readingArgs = true;
					else if(c == ',' || c == ')')
					{
						buffer[bufferPosition] = '\0';
						//printf("argument %d: %s\n", argCount, buffer);
						switch(argCount)
						{
							case 0:
								b->name = cloneCString(buffer);
								break;
							case 1:
								b->structName = cloneCString(buffer);
								break;
							case 2:
								b->updateProcName = cloneCString(buffer);
								break;
							case 3:
								b->drawProcName = cloneCString(buffer);
								break;
						}
						bufferPosition = 0;
						argCount++;
						continue;
					}
					else if(readingArgs)
						buffer[bufferPosition++] = c;
				}
				
				// If no more room remains for behavior info then end search
				
				if(behaviorCount >= MAX_BEHAVIORS)
				{
					printf("Warning, no more room for behavior info structs\n.");
					break;
				}
			}
		}
	}
	
	//fileString = malloc(10000);
	//fileStringPosition = 0;
	
	FILE* file = fopen("src/generated.h", "wab");
	if(file)
	{
		fprintf(file, "\
typedef struct __attribute__((__packed__)) Behaviors\n\
{\n\
	int dataStart;\n");
		
		for(int i = 0; i < behaviorCount; ++i)
			fprintf(file, "\t%s* %s;\n", behaviors[i].structName, behaviors[i].name);
			
		fprintf(file, "} Behaviors;\n\n");
		
		fprintf(file, "\
typedef struct __attribute__((__packed__)) BehaviorPools\n\
{\n\
	int dataStart;\n");
		
		for(int i = 0; i < behaviorCount; ++i)
			fprintf(file, "\tBehaviorPool %s;\n", behaviors[i].name);
			
		fprintf(file, "} BehaviorPools;\n\n");
	}
	fclose(file);
	
	file = fopen("src/generated.c", "wab");
	if(file)
	{
		fprintf(file, "\
void initBehaviorPools(BehaviorPools* behaviorPools)\n\
{");
		for(int i = 0; i < behaviorCount; ++i)
		{
			fprintf(file, "\n\
	initBehaviorPool(&behaviorPools->%s, \n\
		sizeof(%s), %s, %s);\n",
				behaviors[i].name, behaviors[i].structName,
				behaviors[i].updateProcName,
				behaviors[i].drawProcName);
		}
		fprintf(file, "}");
	}
	fclose(file);
}
