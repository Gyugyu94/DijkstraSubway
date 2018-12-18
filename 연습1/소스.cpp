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

struct connectedStation {
	char *id;
	int dist;
	struct connectedStation *next;
};

struct vertex {
	char *id;
	struct connectedStation *connected;
	struct vertex *next;
};

struct dijkstra {
	char *id;
	int found;
	int dist;
	char *prev;
};

struct node {
	char *id;
	struct node *next;
};

struct node *head = 0;

struct dijkstra *dtable = 0;

struct vertex *graph = 0;


struct station *stations = 0;
int vertex_count = 0;

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

	//printf("%s %s %s \n", new_one->id, new_one->line, new_one->name);

}

void parseStationInfo(char *_buf)
{
	char name[100];
	char id[100];
	char line[100];

	sscanf(_buf, "%s %s %s\n", id, name, line);
	addStationNode(id, name, line);
	vertex_count += 1;
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

	new_one->connected = 0;
	new_one->next = 0;

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

void addNewEdge(char *fromStation, char * toStation, int dist)
{
	struct vertex *v = findVertexById(fromStation);

	struct connectedStation *new_one = (struct connectedStation *)malloc(sizeof(struct connectedStation));
	new_one->id = (char *)malloc(strlen(toStation) + 1);
	strcpy(new_one->id, toStation);

	new_one->dist = dist;

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

	return;


}

void parseGraphInfo(char *_buf)
{
	char fromStation[100];
	char toStation[100];
	int distance;

	sscanf(_buf, "%s %s %d\n", fromStation, toStation, &distance);

	addNewEdge(fromStation, toStation, distance);
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
				addNewEdge(temp->id, follow->id,0);
				addNewEdge(follow->id, temp->id,0);
			}
			follow = follow->next;
		}
		temp = temp->next;
	}
}

void initDtable(void)
{
	struct dijkstra *temp = (struct dijkstra *)malloc(sizeof(struct dijkstra)*vertex_count);
	struct vertex *temp2 = graph;

	for (int i = 0; i < vertex_count; i++)
	{
		temp[i].id = temp2->id;
		temp[i].found = 0;
		temp[i].dist = INT_MAX;
		temp[i].prev = 0;

		temp2 = temp2->next;

	}
	dtable = temp;
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
int findNextShortestVertex(void)
{
	int shortestidx = -1;
	int shortestDistance = INT_MAX;

	for (int i = 0; i < vertex_count; i++)
	{
		if (dtable[i].found == 0 && dtable[i].dist < shortestDistance)
		{
			shortestDistance = dtable[i].dist;
			shortestidx = i;
		}
	}
	return shortestidx;		
}

int checkDuplication(char *_id)
{
	struct node *temp = head;

	while (temp != 0)
	{
		if (strcmp(findNameById(temp->id),findNameById(_id)) == 0)
		{
			return 1;
		}
		temp = temp->next;
	}
	return 0;
}

void push_subway(char *_id)
{
	if(checkDuplication(_id) == 1)return;

	struct node *new_one = (struct node *)malloc(sizeof(struct node));
	new_one->id = (char *)malloc(strlen(_id) + 1);
	strcpy(new_one->id, _id);

	new_one->next = head;
	head = new_one;
	return;
}


void runDijkstra(char *_src, char *_dst)
{
	int idx = 0;
	char *src_id = findIdByName(_src);
	char *dst_id = findIdByName(_dst);

	for (int i = 0; i < vertex_count; i++)
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
		int k = findNextShortestVertex();

		if (k == -1)
		{
			break;
		}

		dtable[k].found = 1;

		struct vertex *temp = findVertexById(dtable[k].id);
		struct connectedStation *connected = temp->connected;
		int connected_idx = 0;

		while (connected != 0)
		{
			for (int i = 0; i < vertex_count; i++)
			{
				if (strcmp(dtable[i].id, connected->id) == 0)
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
	char *prev_id = dst_id;
	while (1)
	{
		for (int i = 0; i < vertex_count; i++)
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
void showQuery(void)
{
	struct node *temp = head;

	while (temp != 0)
	{
		printf("%s\n", findNameById(temp->id));
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

	buf[0] = '\0';
	
	while (!feof(f))
	{
		fgets(buf, 99, f);
		parseGraphInfo(buf);
		buf[0] = '\0';
		
	}

	correctConnectionInformation();

	initDtable();
	
	char src[20];
	char dst[20];

	printf("시작 역: ");
	scanf("%s", src);
	printf("도착 역: ");
	scanf("%s", dst);

	runDijkstra(src,dst);

	showQuery();
	
}