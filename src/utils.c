int randomi(int max)
{
	return (rand() % max);
}

float randomf(float max)
{
	return ((float)rand() / RAND_MAX) * max;
}

int randomBetweeni(int min, int max)
{
	int delta = max - min;
	return min + randomi(delta+1);
}

float randomBetweenf(float min, float max)
{
	int delta = max - min;
	return min + randomf(delta);
}

void quickSorti(int* values, int valueCount)
{
	if(valueCount < 2)
		return;
		
	int pivot = values[valueCount/2];
	
	int i, j;
	for(i = 0, j = valueCount - 1; ; ++i, --j)
	{
		while(values[i] < pivot)
			++i;
			
		while(pivot < values[j])
			--j;
			
		if(i >= j)
			break;
			
		int temp = values[i];
		values[i] = values[j];
		values[j] = temp;
	}
		
	quickSorti(values, i);
	quickSorti(values + i, valueCount - i);
}

float degToRad(float degrees)
{
	return degrees * (PI / 180);
}

float clampf(float value, float min, float max)
{
	return value < min ? min : value > max ? max : value;
}

Color createColor(byte r, byte g, byte b, byte a)
{
	Color color = {r, g, b, a};
	return color;
}

Pointi createPointi(int x, int y)
{
	Pointi point = {x, y};
	return point;
}

Pointf createPointf(float x, float y)
{
	Pointf point = {x, y};
	return point;
}

Pointf clampPointf(Pointf point, float x1, float y1, float x2, float y2)
{
	point.x = clampf(point.x, x1, x2);
	point.y = clampf(point.y, y1, y2);
	
	return point;
}

SDL_Point getSdlPointi(Pointi point)
{
	SDL_Point sdlPoint = {point.x, point.y};
	return sdlPoint;
}

Pointf addPointfs(Pointf a, Pointf b)
{
	return createPointf(a.x + b.x, a.y + b.y);
}

Pointf subtractPointfs(Pointf a, Pointf b)
{
	return createPointf(a.x - b.x, a.y - b.y);
}

Pointf scalePointf(Pointf point, float scaler)
{
	return createPointf(point.x * scaler, point.y * scaler);
}

float distanceBetweenPointfs(Pointf first, Pointf second)
{
	Pointf delta = createPointf(second.x - first.x, second.y - first.y);
	return sqrt(delta.x * delta.x + delta.y * delta.y);
}

Pointf normalizePointf(Pointf point)
{
	float distance = distanceBetweenPointfs(createPointf(0, 0), point);
	if(distance > 0)
		return scalePointf(point, 1.0f / distance);
	return point;
}

Pointi scalePointi(Pointf point, float scaler)
{
	return createPointi((int)((float)point.x * scaler), 
						(int)((float)point.y * scaler));
}

Rectanglei createRectanglei(int x, int y, int width, int height)
{
	Rectanglei rectangle = {x, y, width, height};
	return rectangle;
}

void intToFixedSizeString(int value, int digits, char* dest)
{
	dest[digits] = 0;
	for(int i = 0; i < digits; ++i)
		dest[digits - i - 1] = '0' + ((value / (int)pow((double)10, (double)i))%10);
}
