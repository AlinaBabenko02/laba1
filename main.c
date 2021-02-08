#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#define ifm if(master!=0) 
#define ifs if(slave!=0)
#define ifi if(index!=0)
#define mop fopen_s(&master, "master.txt", "r+");
#define iop fopen_s(&index, "indexes.txt", "r+");
#define sop fopen_s(&slave, "slave.txt", "r+");
FILE* master;
FILE* slave;
FILE* index;

int compare(const void* a, const void* b)
{
	const char* a1 = a;
	const char* b1 = b;
	if (a1[0] == '\0') if (b1[0] != '\0') return 1;
	if (b1[0] == '\0') if (a1[0] != '\0') return -1;
	if (b1[0] == '\0' && a1[0] == '\0') return 0;
	return strcmp(a1, b1);
}


int compare1(const void* a, const void* b)
{
	char* idx_table = b;
	char* digit = a;
	char* pos1 = strchr(idx_table, ' ');
	int pos = pos1 - idx_table;
	char index[6];
	strncpy_s(index, 6, idx_table, pos);
	if (idx_table[0] == '\0') return -1;
	return strcmp(digit, index);
}


int erase(char str[20][13], int id, int size)
{
	char digit[6];
	sprintf_s(digit, 6, "%d", id);
	for (int i = 0; i < size; i++)
		if (compare1(digit, str[i]) == 0)
		{
			for (int j = i; j < size; j++)
				if (j + 1 != 20) strcpy_s(str[j], 13, str[j + 1]);
				else str[j][0] = '\0';
			size--;
			break;
		}
	return size;
}


void WriteIndexTable(char indx[20][13], int size)
{
	fopen_s(&index, "indexes.txt", "w");
	ifi fseek(index, 0L, SEEK_SET);
	for (size_t i = 0; i < size; i++)
	{
		ifi fwrite(indx[i], 1, 13, index); //Переписываю индексную таблицу в сортированном виде
	}
	ifi fclose(index);
}


int fillIndexesMass(int* indexes, char* line, char* file)
{
	FILE* f;
	fopen_s(&f, file, "r+");
	fseek(f, 0L, SEEK_SET);
	fread(line, 1, 150, f); //считывание 1 строки
	char* p_line = line;
	for (size_t i = 0; i < 20; i++)
	{
		indexes[i] = -1; //заполняю массив свободных индексов -1
	}
	int counter = 0;
	for (size_t i = 0; i < 20; i++)
	{
		if (sscanf_s(p_line, "%d,", indexes) == 0) break;
		while (*p_line != ',' && *p_line != '\0') p_line++; //Заполняю массив свободных индексов реальными значениями
		if (*p_line == '\0') break; else
			p_line++;
		counter++;
		indexes++;
	}
	fclose(f);
	return counter;
}

////Возвращает индекс на 1 больше, чем индекс у последней записи
//int GetLastIndex(char* line, char* file)
//{
//	FILE* f;
//	fopen_s(&f, file, "r+");
//	int curindx;
//	 fseek(f, -150, SEEK_END);    //Если нет свободных индексов - беру последний индекс и увеличиваю на 1
//	 fread(line, sizeof(char), 150, f);
//	sscanf_s(line, "%d,", &curindx);
//	curindx++;
//	fclose(f);
//	return curindx;
//}

//Получает позицию символа "очистки"
int GetGarbIdx(char* str)
{
	char* p_pos = strrchr(str, ',');
	int GarbIndx = (p_pos - str) + 1;
	return GarbIndx;

}





void DoNewLine(char* new_line, char* comma, char* line, char* new_value)
{
	comma++;
	char* p_end = strchr(comma, ',');
	strncpy_s(new_line, 150, line, comma - line);
	strcat_s(new_line, 150, new_value);
	strcat_s(new_line, 150, p_end);
}


int isNumber(char* number, int size)
{
	int result = 1;
	for (size_t i = 0; i < size; i++)
	{
		if (number[i] < '0' || number[i]>'9') result = 0;
	}
	return result;
}

//Записывает индексную таблицу в оперативную память
int fillIdxTable(char table[20][13])
{
	for (size_t i = 0; i < 20; i++)
	{
		table[i][0] = '\0';
	}
	fopen_s(&index, "indexes.txt", "r+");
	ifi fseek(index, 0L, SEEK_END);
	int size = ftell(index) / 13;
	ifi fseek(index, 0L, SEEK_SET);
	for (int i = 0; i < size; i++)
	{
		ifi fread(table[i], 1, 13, index);
	}
	ifi fclose(index);
	return size;
}

//Возарщает координату в мастер файле, используя бинарный поиск по индексной таблице
int posInMaster(int pos)
{
	char idx_table[20][13];
	fillIdxTable(idx_table);
	char digit[6];
	sprintf_s(digit, 6, "%d", pos);
	char* bool = bsearch(digit, idx_table, 20, 13, compare1);
	if (bool == NULL) return 0;
	int masId = (bool - idx_table) / 13;
	int kostyl, weneed;
	sscanf_s(idx_table[masId], "%d %d", &kostyl, &weneed);
	return weneed;
}

int findLastSlavePos(int index, char* line, int a)
{
	long int pos = posInMaster(index);
	mop;
	ifm fseek(master, pos, 0);
	ifm fread(line, 1, 147, master);
	ifm fclose(master);
	char* p_line = line;
	for (int i = 0; i < 4; i++)
	{
		p_line = strchr(p_line, ',');
		p_line++;
	}
	int pos_slave, pos_pre_last = 0, pos_last = 0;
	sscanf_s(p_line, "%d", &pos_slave);
	sop;
	while (pos_slave != 0)
	{
		pos_pre_last = pos_last;
		pos_last = pos_slave;
		ifs fseek(slave, pos_slave, 0);
		ifs fread(line, 1, 147, slave);
		p_line = line;
		for (int i = 0; i < 2; i++)
		{
			p_line = strchr(p_line, ',');
			p_line++;
		}
		sscanf_s(p_line, "%d", &pos_slave);
	}
	ifs fclose(slave);
	return (a == 1) ? pos_last : pos_pre_last;
}


//Возвращает первый свободный идекс, переписывает строку свободных индексов
int GetIndexFromFree(char* line, int* indexes, char* file)
{
	int curindx;
	FILE* f;
	fopen_s(&f, file, "r+");
	curindx = indexes[0]; //присваиваю индексу свободный индекс
	char* p_line = line;
	if (indexes[1] == -1) sprintf_s(line, 147, "%d,", curindx + 1); //если всего 1 свободный индекс - заменяю первый символ
	for (int i = 1; i < 20; i++)
	{
		if (indexes[i] == -1) break;
		if (sprintf_s(p_line, 147 - (p_line - line), "%d,", indexes[i]) < 0) printf("Error\n"); //записываю в line оставшиеся свободные индексы
		while (*p_line != '\0' && *p_line != ',') p_line++;
		if (*p_line != '\0')     //Двигаю указатель на место после , чтобы записать следующий индекс
			p_line++;
	}
	char a[2] = "\0\n";
	fseek(f, 0L, SEEK_SET);
	fwrite(line, 1, 147, f);
	fwrite(a, 1, 2, f);       //вписываю новую строку индексов
	fclose(f);
	return curindx;
}


void updateIndexes(int index, char* file)
{
	FILE* f;
	fopen_s(&f, file, "r+");
	char line[141];
	fseek(f, 0, SEEK_SET);
	fread(line, 1, 140, f);
	char new_line[150];
	sprintf_s(new_line, 147, "%d,%s", index, line);
	fseek(f, 0, SEEK_SET);
	fwrite(new_line, 1, 147, f);
	fclose(f);
}


void insert_m(const char firm[35], const char num_of_sts[35], const char Mod_nm[35])
{
	char indx[20][13];
	int size = fillIdxTable(indx); //сохранение индексной таблицы в оперативу
	int curindx;
	char line[150];
	int indexes[20];
	fillIndexesMass(indexes, line, "master.txt");
	curindx = GetIndexFromFree(line, indexes, "master.txt");
	mop;
	char a[2] = "\0\n";
	fseek(master, curindx * 150, 0);
	sprintf_s(line, 147, "%d,%s,%s,%s,%d,%d", curindx, firm, num_of_sts, Mod_nm, 0, 1); //Записываю в строку то, что нужно вставить      
	ifm fwrite(line, 1, 147, master); //Добавляю в конец файла новую запись
	ifm fwrite(a, 1, 2, master);
	sprintf_s(indx[size], 13, "%d %d", curindx, ftell(master) - 150); //Добавляю в массив индексной таблицы новую запись
	qsort(indx, 20, 13, compare);      //Сортирую данные в индексной таблице
	size++;
	WriteIndexTable(indx, size);
	ifm fclose(master);
}


void insert_s(int modID)
{
	if (posInMaster(modID) == 0) printf("Not found note in master file\n");
	else
	{
		int curindx;
		char line[150];
		int indexes[20];
		fillIndexesMass(indexes, line, "slave.txt");
		curindx = GetIndexFromFree(line, indexes, "slave.txt");
		sop;
		char a[] = "\0\n";
		sprintf_s(line, 147, "%d,%d,%d,%d", curindx, modID, 0, 1);
		ifs fseek(slave, curindx * 150, 0);
		long int pos_written = ftell(slave);
		ifs fwrite(line, 1, 147, slave);
		ifs fwrite(a, 1, 2, slave);
		ifs fclose(slave);
		int pos_slave = findLastSlavePos(modID, line, 1);
		if (pos_slave == 0)
		{
			long int pos_master = posInMaster(modID);
			mop;
			ifm fseek(master, pos_master, 0);
			char line[150], new_line[150], new_value[15];
			char* p_line = line;
			ifm fread(line, 1, 149, master);
			ifm fseek(master, pos_master, 0);
			for (int i = 0; i < 4; i++)
			{
				p_line = strchr(p_line, ',');
				p_line++;
			}
			p_line--;
			sprintf_s(new_value, 15, "%d", pos_written);
			DoNewLine(new_line, p_line, line, new_value);
			ifm fwrite(new_line, 1, 147, master);
			ifm fclose(master);
		}
		else
		{
			char* p_line = line;
			p_line = strchr(p_line, ',');
			p_line++;
			p_line = strchr(p_line, ',');
			char new_value[15], new_line[150];
			sprintf_s(new_value, 15, "%d", pos_written);
			DoNewLine(new_line, p_line, line, new_value);
			sop;
			ifs fseek(slave, pos_slave, 0);
			ifs fwrite(new_line, 1, 147, slave);
			ifs fclose(slave);
		}
	}
}


int get_m(int index)
{
	char index_table[20][13];
	char line[150];
	fillIdxTable(index_table);
	int pos = posInMaster(index);
	if (pos == 0)
	{
		printf_s("No that note in master file\n");
		return 0;
	}
	mop;
	ifm fseek(master, pos, SEEK_SET);
	ifm fread(line, 1, 150, master);
	printf("%s\n", line);
	fclose(master);
	return pos;
}


int get_s(int index)
{
	sop;
	fseek(slave, 0, 2);
	if (index * 150 > ftell(slave))
	{
		printf("Can't find such note\n");
		return 0;
	}
	fseek(slave, index * 150, 0);
	char line[150];
	fread(line, 1, 150, slave);
	int garbindx = GetGarbIdx(line);
	if (line[garbindx] == '0')
	{
		printf("Can't find such note\n");
		return 0;
	}
	printf(line);
	fclose(slave);
	return index * 150;
}


void del_s(int index)
{
	int pos = get_s(index);
	if (pos == 0) return;
	char line[150];
	sop;
	ifs fseek(slave, pos, SEEK_SET);
	ifs	fread(line, 1, 149, slave);
	ifs fclose(slave);
	int modID, pos_next;
	sscanf_s(line, "%d,%d,%d", &index, &modID, &pos_next);
	long int pos_last = findLastSlavePos(modID, line, 1);

	long int pos_pre_last = findLastSlavePos(modID, line, 0);
	if (pos_pre_last == 0)
	{
		long int pos_master = posInMaster(modID);
		char line1[150], new_line[150], new_value[15];
		mop;
		ifm fseek(master, pos_master, 0);
		ifm fread(line1, 1, 149, master);
		ifm fclose(master);
		char* p_line = line1;
		for (int i = 0; i < 4; i++)
		{
			p_line = strchr(p_line, ',');
			p_line++;
		}
		p_line--;
		sprintf_s(new_value, 15, "%d", 0);
		DoNewLine(new_line, p_line, line1, new_value);
		mop;
		ifm fseek(master, pos_master, 0);
		ifm fwrite(new_line, 1, 147, master);
		ifm fclose(master);
		int garbIndx = GetGarbIdx(line);
		line[garbIndx] = '0';
		sop;
		fseek(slave, pos_last, 0);
		fwrite(line, 1, 149, slave);
		fclose(slave);
	}
	else
	{
		int garbIndx = GetGarbIdx(line);
		line[garbIndx] = '0';
		sop;
		fseek(slave, pos_last, SEEK_CUR);
		fwrite(line, 1, 149, slave);
		fclose(slave);
		sop;
		ifs fseek(slave, pos_pre_last, SEEK_SET);
		ifs	fread(line, 1, 149, slave);
		ifs fclose(slave);
		char new_line[150];
		char* p_line = line;
		p_line = strchr(p_line, ',');
		p_line++;
		p_line = strchr(p_line, ',');
		DoNewLine(new_line, p_line, line, "0");
		sop;
		ifs fseek(slave, pos_pre_last, 0);
		fwrite(new_line, 1, 147, slave);
		ifs fclose(slave);
	}
	updateIndexes(pos_last / 150, "slave.txt");

}


void del_m(int index_)
{
	int pos = posInMaster(index_);
	if (pos == 0)
	{
		printf("No such note in file\n");
		return;
	}
	char line[149];
	long int posit = 150;
	int slave_pos = findLastSlavePos(index_, line, 1);
	while (slave_pos != 0)
	{
		del_s(slave_pos / 150);
		slave_pos = findLastSlavePos(index_, line, 1);
	}
	mop;
	fseek(master, pos, SEEK_SET);
	fread(line, 1, 149, master);
	int GarbIndx = GetGarbIdx(line);
	line[GarbIndx] = '0';
	fseek(master, pos, SEEK_SET);
	fwrite(line, 1, 147, master);
	fclose(master);
	int freeindx;
	sscanf_s(line, "%d", &freeindx);
	updateIndexes(freeindx, "master.txt");
	char indx_[20][13];
	int size = fillIdxTable(indx_);
	size = erase(indx_, index_, size);
	WriteIndexTable(indx_, size);
}


void update_m(int index_, char* field, char* new_value)
{
	int pos_m = posInMaster(index_);
	if (pos_m == 0)
	{
		printf("No such note in master file(update_m)\n");
		return;
	}
	mop;
	fseek(master, pos_m, 0);
	char line[150];
	fread(line, 1, 149, master);
	fseek(master, -150, 1);
	char new_line[150];
	char* p_line = strchr(line, ',');
	if (strcmp(field, "firm") == 0)
	{
	}
	else
		if (strcmp(field, "number_of_seats") == 0)
		{
			if (isNumber(new_value, strlen(new_value)) == 0)
			{
				printf("number_of_seats isn't number. Error\n");
				fclose(master);
				return;
			}
			p_line++;
			p_line = strchr(p_line, ',');

		}
		else if (strcmp(field, "model_name"))
		{
			p_line++;
			p_line = strchr(line, ',');
			p_line++;
			p_line = strchr(line, ',');
		}
		else
		{
			printf("Didn't find that field\n");
			fclose(master);
			return;
		}
	DoNewLine(new_line, p_line, line, new_value);
	fwrite(new_line, 1, 147, master);
	char a[2] = "\0\n";
	fwrite(a, 1, 2, master);
	fclose(master);
}


void update_s(int index_, char* field, char* new_value)
{
	;
}


int calc_m()
{
	mop;
	char line[150];
	int counter = 0;
	while (1)
	{
		fread(line, 1, 149, master);
		if (feof(master) == 1) break;
		if (line[GetGarbIdx(line)] == '1') counter++;
		printf(line);
	}
	fclose(master);
	return counter;
}


int calc_s()
{
	mop;
	char line[150];
	int counter = 0;
	while (1)
	{
		fread(line, 1, 149, slave);
		if (feof(slave) == 1) break;
		if (line[GetGarbIdx(line)] == '1') counter++;
		printf(line);
	}
	fclose(slave);
	return counter;
}


int calc_s_m(int id_master)
{
	int pos_master = posInMaster(id_master);
	if (pos_master == 0)
	{
		return 0;
	}
	mop;
	char line[150];
	fseek(master, pos_master, 0);
	fread(line, 1, 149, master);
	fclose(master);
	char* p_line = line;
	for (int i = 0; i < 4; i++)
	{
		p_line = strchr(p_line, ',');
		p_line++;
	}
	int pos_slave;
	sscanf_s(p_line, "%d", &pos_slave);
	if (pos_slave == 0) return 0;
	int counter = 0;
	while (pos_slave != 0)
	{
		sop;
		counter++;
		ifs
			fseek(slave, pos_slave, 0);
		ifs
			fread(line, 1, 149, slave);
		char* p_line = line;
		for (int i = 0; i < 2; i++)
		{
			p_line = strchr(p_line, ',');
			p_line++;
		}
		sscanf_s(p_line, "%d", &pos_slave);
		fclose(slave);
	}
	fclose(slave);
	return counter;
}


void ut_m()
{
	mop;
	char line[150];
	while (1)
	{
		fread(line, 1, 149, master);
		if (feof(master) == 1) break;
		printf(line);
	}
	fclose(master);
}


void ut_s()
{
	sop;
	char line[150];
	while (1)
	{
		fread(line, 1, 149, slave);
		if (feof(slave) == 1) break;
		printf(line);
	}
	fclose(slave);
}


int main()
{

	int f[6] = { 6,1,2,3,4,5 };
	qsort(f, 6, sizeof(int), compare);
	fopen_s(&index, "indexes.txt", "w");
	fopen_s(&master, "master.txt", "w");
	fopen_s(&slave, "slave.txt", "w");
	char a1[147] = "1,";
	char a[2] = "\0\n";
	fwrite(a1, 1, 147, master);
	fwrite(a1, sizeof(char), 147, slave);
	fwrite(a, 2, 1, master);
	fwrite(a, 2, 1, slave);
	fclose(master);
	fclose(slave);
	fclose(index);
	insert_m("Firm1", "21", "name1");
	insert_m("Firm2", "22", "name2");
	insert_m("Firm3", "23", "name3");
	insert_m("Firm4", "24", "name4");
	insert_m("Firm5", "25", "name5");
	insert_s(1);
	insert_s(2);
	insert_s(1);
	del_s(3);
	del_s(1);
	insert_s(1);
	insert_s(1);
	insert_m("Firm7751", "21", "name1");
	get_m(1);
	get_s(1);
	update_m(1, "number_of_seats", "fgsdf");
	printf("%d", calc_s_m(1));
	return 0;
}