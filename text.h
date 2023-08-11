#ifndef TEXT_H
#define TEXT_H

void 
wrapString(char line[], 
		   char newline[], 
		   int width, 
		   int len)
{

	int wl;
	int linewidth = 0;
	int col;

	for(col = 0; col < len; ++col){
		wl = 0;
		newline[col] = line[col];
		++linewidth;

		for(wl = 0; line[col+wl+1] != ' '; ++wl);

		if(linewidth+wl > width){
			newline[col] = '\n';
			linewidth = 0;
		}
	
	}
	newline[col] = 0;
}

#endif

