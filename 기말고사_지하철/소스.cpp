#include <stdio.h>
#include <stdlib.h>
#include <string.h>
char* findStationNameById(char *id);
// Station ������ �����ϱ� ���� SLL
struct Station
{
	char *id;
	char *name;
	char *line;
	struct Station *next;
};
struct Station *stations = 0;

struct ConnectedStation
{
	char *id;
	int distance;
	struct ConnectedStation *next;
};
struct Vertex
{
	char *id;
	struct ConnectedStation *connected;
	struct Vertex *next;
};
struct Vertex *graph = 0;

//stack

struct stack_node
{
	char *id;
	struct stack_node *next;
};

struct stack_node *query = 0;
struct stack_node *subway = 0;

struct stack_node *top = 0;
struct stack_node *visited = 0;
int DFS_count = 0;

struct dijkstra
{
	int found;
	int dist;
	char *prev;
	char *id;
};
struct dijkstra *dtable = 0;
int dijkstra_dst_idx = 0;


void subway_push(char *id)
{
	struct stack_node *new_one = (struct stack_node *)malloc(sizeof(struct stack_node));
	new_one->id = id;
	new_one->next = 0;

	new_one->next = query;
	query = new_one;
	return;

}

void push(char *id)
{
	struct stack_node *new_one = (struct stack_node *)malloc(sizeof(struct stack_node));
	new_one->id = id;
	new_one->next = 0;

	new_one->next = top;
	top = new_one;
	return;

}
struct stack_node* pop(void)
{
	struct stack_node *temp = top;
	if (top != 0)
	{
		top = top->next;
	}
	return temp;

}


void addStationNode(char *_id, char *_name, char *_line)
{
	struct Station *new_one = (struct Station *)malloc(sizeof(struct Station));

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

	struct Station *temp = stations;
	while (temp->next != 0)
	{
		temp = temp->next;
	}
	temp->next = new_one;

}

// �� �̸��� �Է��ϸ�,
// ���� ������ȣ�� ��ȯ�Ѵ�.
// ���̸��� ã�� ���ϸ�, null pointer�� ��ȯ
char *findIdByName(char *_name)
{
	struct Station *temp = stations;

	while (temp != 0)
	{
		if (strcmp(temp->name, _name) == 0)
		{
			return temp->id;
		}
		temp = temp->next;
	}
	return 0;
}

void parseStationInfo(char *_buf)
{
	char id[100];
	char name[100];
	char line[100];

	sscanf(_buf, "%s %s %s\n", id, name, line);

	addStationNode(id, name, line);
	DFS_count += 1;

}

struct Vertex* findVertexById(char *_id)
{
	struct Vertex *temp = graph;

	while (temp != 0)
	{
		if (strcmp(temp->id, _id) == 0)
		{
			return temp;   // ã�� ���,
		}
		temp = temp->next;
	}

	// �� ã�� ���,,,,
	// Vertex�� ���� �Ҵ�޾ƾƼ�, 
	struct Vertex *new_one = (struct Vertex *)malloc(sizeof(struct Vertex));
	new_one->id = (char *)malloc(strlen(_id) + 1);
	strcpy(new_one->id, _id);
	new_one->connected = 0; /// �߿�!!!!!!
	new_one->next = 0;

	//�� ���� �߰��Ѵ�.
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

void addNewEdge(char *fromStation, char *toStation, int distance)
{

	struct Vertex *v = findVertexById(fromStation);

	struct ConnectedStation *new_one =
		(struct ConnectedStation *)malloc(sizeof(struct ConnectedStation));
	new_one->distance = distance;
	new_one->id = (char *)malloc(strlen(toStation) + 1);
	strcpy(new_one->id, toStation);
	new_one->next = 0;

	// ���� v�� �����ʿ� ���δ�.
	if (v->connected == 0)
	{
		v->connected = new_one;
		return;
	}
	struct ConnectedStation *temp = v->connected;
	while (temp->next != 0)
	{
		temp = temp->next;
	}
	temp->next = new_one;
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
	struct Station *temp = stations;

	while (temp->next != 0)
	{
		struct Station *follower = temp->next;
		while (follower != 0)
		{
			// [2] temp�� follower�� �̸��� ���� ��쿡��
			if (strcmp(temp->name, follower->name) == 0)
			{
				addNewEdge(temp->id, follower->id, 0); // edge�� �߰� temp->follower
				addNewEdge(follower->id, temp->id, 0); // edge�� �߰� follower->temp
			}
			follower = follower->next;
		}
		temp = temp->next;
	}
}


char* findStationNameById(char *id)
{
	struct Station *temp = stations;
	while (temp != 0)
	{
		if (strcmp(temp->id, id) == 0)
		{
			return temp->name;
		}
		temp = temp->next;
	}
}


void initDtable(void)
{
	struct dijkstra *temp = (struct dijkstra *)malloc(sizeof(struct dijkstra)*DFS_count);
	struct Vertex *temp2 = graph;
	for (int i = 0; i < DFS_count; i++)
	{
		temp[i].id = temp2->id;
		temp[i].found = 0;
		temp[i].dist = 200000000;
		temp[i].prev = 0;
		temp2 = temp2->next;

	}

	dtable = temp;


}


int findNextShortestVertex(void)
{
	int shortestDistance = 2000000000;
	int shortestidx = -1;
	for (int i = 0; i < DFS_count; i++)
	{
		if (dtable[i].found == 0 && dtable[i].dist < shortestDistance)
		{
			shortestidx = i;
			shortestDistance = dtable[i].dist;
		}
	}

	return shortestidx;
}

void runDijkstra(char *_src, char *_dst)
{
	int idx = 0;
	int connected_idx = 0;
	char *src_id = findIdByName(_src);
	char *dst_id = findIdByName(_dst);

	for (int i = 0; i < DFS_count; i++)
	{
		if (strcmp(dtable[i].id, src_id) == 0)
		{
			idx = i;
			break;
		}
	}
	dtable[idx].dist = 0; // pseudo code algorithm's 2nd step

	while (1) // Dijkstra pseudo code�� 3�ܰ�.
	{
		//found = 0 �̸鼭, dist�� �ּ��� ���� ã�´�.
		int k = findNextShortestVertex();

		if (k == -1)// ��� �ִܰ�θ� �� ã�����Ƿ� ����.
		{
			break;
		}

		//  found == true �� ����
		dtable[k].found = 1;
		//  k �� neighbor�鿡 ���ؼ� dtable �� update



		struct Vertex *temp = findVertexById(dtable[k].id);
		struct ConnectedStation *connected = temp->connected;

		while (connected != 0)
		{
			for (int i = 0; i < DFS_count; i++)
			{
				if (strcmp(dtable[i].id, connected->id) == 0)
				{
					connected_idx = i;
					break;
				}
			}
			if ((dtable[connected_idx].found == 0) &&
				dtable[connected_idx].dist > dtable[k].dist + connected->distance)
			{
				dtable[connected_idx].dist = dtable[k].dist + connected->distance;
				dtable[connected_idx].prev = dtable[k].id;

			}
			connected = connected->next;
		}


	}

	int idx_subway = 0;
	char *prev_id = findIdByName(_dst);

	while (1)
	{
		for (int i = 0; i < DFS_count; i++)
		{
			if (strcmp(dtable[i].id, prev_id) == 0)
			{
				idx_subway = i;
				break;
			}
		}
		//printf("%s \n", findStationNameById(dtable[idx_subway].id));
		subway_push(dtable[idx_subway].id);
		prev_id = dtable[idx_subway].prev;
		if (strcmp(_src, (char *)findStationNameById(dtable[idx_subway].id)) == 0)return;
	}


}

void showQuery(void)
{
	struct stack_node *temp = query;

	while (temp != 0)
	{
		printf("%s\n", findStationNameById(temp->id));
		temp = temp->next;
	}
}

int main(void)
{
	FILE* f;
	f = fopen("seoul.txt", "rt");

	if (f == 0)  // file�� ã�� ���ؼ� ����
	{
		printf("Unable to read file\n");
		return -1;
	}
	char buf[100];

	while (!feof(f))  // file end of file == feof
	{
		fgets(buf, 99, f);

		if (buf[0] == '\n')
		{
			break;
		}
		parseStationInfo(buf); // ���� ���� ������ȣ, �̸�, ȣ����ȣ �и�
		buf[0] = '\0';
	}

	// Graph�� �����ϴ� �κ�
	buf[0] = '\0';  // buffer clear

					// �׷��� ���������� �о �����Ѵ�.
	while (!feof(f))
	{
		fgets(buf, 99, f);
		parseGraphInfo(buf); // ���� ���� ������ȣ, �̸�, ȣ����ȣ �и�
		buf[0] = '\0';
	}

	correctConnectionInformation();


	initDtable();
	runDijkstra((char *)"��õ���Ա�", (char *)"�������");

	showQuery();

	fclose(f);
	return 0;
}