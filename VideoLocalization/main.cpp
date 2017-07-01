



void main()
{
	HANDLE Hcom;
	Hcom = Config_Receive_Procedure(Find_Port_ProceDure());
	Rear = (struct Node*)malloc(sizeof(struct Node));              
	if (!Rear)
	{ }
	Front = CreateQueue();
	Rear->Next = Front;

	LQ_Front = Location_Create();
	LQ_Rear = (struct Location_Node*)malloc(sizeof(struct Location_Node));
	if (!LQ_Rear)
	{ }
	LQ_Rear->Next = LQ_Front;

	CreateThread(NULL, 0, Receive_data, Hcom, 0, NULL);
	CreateThread(NULL, 0, Analyze_data, NULL, 0, NULL);
}
