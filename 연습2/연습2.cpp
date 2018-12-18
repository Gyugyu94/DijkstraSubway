
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
char *findNameById(char *_id);

struct station {
	char *id;
	char *name;
	char *line;
	struct station *next;
};


struct station *stations = 0;
int vertex_cnt = 0;

struct connectedStation {
	char *id;
	int dist;
	struct connectedStation *next;
};

struct node {
	char *id;
	struct node *next;
};

struct node *head = 0;

void push_subway(char *_id)
{
	struct node *new_one = (struct node *)malloc(sizeof(struct node));
	new_one->id = _id;
	new_one->next = 0;

	new_one->next = head;
	head = new_one;

	return;
}

struct vertex {
	char *id;
	struct vertex *next;
	struct connectedStation *connected;
};

struct dijkstra {
	char *id;
	int found;
	int dist;
	char *prev;
};

struct dijkstra *dtable = 0;

struct vertex *graph = 0;

void addStationNode(char *_id, char *_name, char *_line)
{
	struct station *new_one = (struct station *)malloc(sizeof(struct station));
	new_one->id = (char *)malloc(strlen(_id) + 1);
	strcpy(new_one->id, _id);

	new_one->name = (char *)malloc(strlen(_name) + 1);
	strcpy(new_one->name, _name);

	new_one->line = (char *)malloc(strlen(_line) + 1);
	strcpy(new_one->line, _line);

	new_one->next = 0;

	if (stations == 0)
	{
		stations = new_one;
		return;
	}

	struct station *temp = stations;

	while (temp->next != 0)
	{
		temp = temp->next;
	}
	temp->next = new_one;

}

void parseStationInfo(char *_buf)
{
	char id[100];
	char name[100];
	char line[100];

	sscanf(_buf, "%s %s %s\n", id, name, line);

	addStationNode(id, name, line);
	vertex_cnt += 1;


}

struct vertex *findVertexById(char *_id)
{
	struct vertex *temp = graph;

	while (temp != 0)
	{
		if (strcmp(temp->id, _id) == 0)
		{
			return temp;
		}
		temp = temp->next;
	}

	struct vertex *new_one = (struct vertex *)malloc(sizeof(struct vertex));
	new_one->id = (char *)malloc(strlen(_id) + 1);
	strcpy(new_one->id, _id);

	new_one->next = 0;
	new_one->connected = 0;

	if (graph == 0)
	{
		graph = new_one;
		return new_one;
	}

	temp = graph;

	while (temp->next != 0)
	{
		temp = temp->next;
	}

	temp->next = new_one;
	return new_one;

}

void addNewEdge(char *_toStation, char *_fromStation, int dist)
{
	struct vertex *v = findVertexById(_toStation);

	struct connectedStation *new_one = (struct connectedStation *)malloc(sizeof(struct connectedStation));
	new_one->dist = dist;
	new_one->id = (char *)malloc(strlen(_fromStation) + 1);
	strcpy(new_one->id, _fromStation);
	new_one->next = 0;

	if (v->connected == 0)
	{
		v->connected = new_one;
		return;
	}

	struct connectedStation *temp = v->connected;

	while (temp->next != 0)
	{
		temp = temp->next;
	}
	temp->next = new_one;

}

void parseGraphInfo(char *_buf)
{
	char toStation[100];
	char fromStation[100];
	int dist;

	sscanf(_buf, "%s %s %d\n", toStation, fromStation, &dist);

	addNewEdge(toStation, fromStation, dist);
}

void initDtable(void)
{
	struct dijkstra *temp = (struct dijkstra *)malloc(sizeof(struct dijkstra)*vertex_cnt);
	struct station *temp2 = stations;

	for (int i = 0; i < vertex_cnt; i++)
	{
		temp[i].id = temp2->id;
		temp[i].found = 0;
		temp[i].dist = INT_MAX;
		temp[i].prev = 0;

		temp2 = temp2->next;

	}
	dtable = temp;
}


int findShortestIdx(void)
{
	int shortestidx = -1;
	int shortestdistance = INT_MAX;

	for (int i = 0; i < vertex_cnt; i++)
	{
		if (dtable[i].found == 0 && dtable[i].dist < shortestdistance)
		{
			shortestidx = i;
			shortestdistance = dtable[i].dist;
		}
	}

	return shortestidx;
}

char *findIdByName(char *_name)
{
	struct station *temp = stations;

	while (temp != 0)
	{
		if (strcmp(temp->name, _name) == 0)
		{
			return temp->id;
		}
		temp = temp->next;
	}
}

void runDijkstra(char *_src_name, char *_dst_name)
{
	char *src_id = findIdByName(_src_name);
	int idx = 0;

	for (int i = 0; i < vertex_cnt; i++)
	{
		if (strcmp(dtable[i].id, src_id) == 0)
		{
			idx = i;
			break;
		}
	}

	dtable[idx].dist = 0;

	while (1)
	{
		int k = findShortestIdx();

		if (k == -1)
		{
			break;
		}

		dtable[k].found = 1;

		struct vertex *temp = findVertexById(dtable[k].id);
		struct connectedStation *connected = temp->connected;
		int connected_idx = 0;
		if (connected != 0)
		{
			for (int i = 0; i < vertex_cnt; i++)
			{
				if(strcmp(dtable[i].id, connected->id) == 0)
				{
					connected_idx = i;
					break;
				}
			}

			if (dtable[connected_idx].found == 0 && dtable[connected_idx].dist > dtable[k].dist + connected->dist)
			{
				dtable[connected_idx].dist = dtable[k].dist + connected->dist;
				dtable[connected_idx].prev = dtable[k].id;
			}
			connected = connected->next;
		}
	}


	int idx_subway = 0;
	char *prev_id = findIdByName(_dst_name);



	while (1)
	{
		for (int i = 0; i < vertex_cnt; i++)
		{
			if (strcmp(dtable[i].id, prev_id) == 0)
			{
				idx_subway = i;
				break;
			}
		}

		push_subway(dtable[idx_subway].id);
		prev_id = dtable[idx_subway].prev;
		if (strcmp(src_id, dtable[idx_subway].id) == 0) break;
	}

}



char *findNameById(char *_id)
{
	struct station *temp = stations;

	while (temp != 0)
	{
		if (strcmp(temp->id, _id) == 0)
		{
			return temp->name;
		}
		temp = temp->next;
	}


}

void showSubway(void)
{
	struct node *temp = head;

	while (temp != 0)
	{
		printf("%s \n", findNameById(temp->id));
		temp = temp->next;
	}
}


void correctConnectionInformation(void)
{
	struct station *temp = stations;

	while (temp->next != 0)
	{
		struct station *follow = temp->next;

		while (follow != 0)
		{
			if (strcmp(temp->name, follow->name) == 0)
			{
				addNewEdge(temp->id, follow->id, 0);
				addNewEdge(follow->id, temp->id, 0);
			}
			follow = follow->next;
		}
		temp = temp->next;
	}
}



int main(void)
{
	FILE *f = fopen("seoul.txt", "rt");
	char buf[100];
	while (!feof(f))
	{
		fgets(buf, 99, f);
		if (buf[0] == '\n')
		{
			break;
		}
		
		parseStationInfo(buf);
		buf[0] = '\0';

	}
	printf("%d", vertex_cnt);
	buf[0] = '\0';

	while (!feof(f))
	{
		fgets(buf, 99, f);
		
		parseGraphInfo(buf);
	}

	correctConnectionInformation();



	initDtable();

	char src[20];
	char dst[20];

	/*printf("시작 역: ");
	scanf("%s", src);
	printf("도착 역: ");
	scanf("%s", dst);
*/
	runDijkstra((char *)"부평시장", (char *)"인천대입구");

	showSubway();

}