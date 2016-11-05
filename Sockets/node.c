struct process_node {
  int value;
  pid_t left_son;
  pid_t right_son;
} node_t;

void create_node(node_t* node, int val)
{
  if(node == NULL)
  {
    pid_t pid = fork();
    if(pid == 0)
    {
      node = malloc(sizeof(node_t));
      (*node).value = val;
    }
  }else
  {
    if(val < (*node).value)
    {
      create_node((*node).left_son, val);
    }else if (val > (*node).value)
    {
      create_node((*node).right_son, val);
    }
  }
}

int search_node(node_t* node, int val)
{
  if(node == NULL)
  {
    return -1;
  }

  if((*node).value == val)
  {
    printf("Found! My pid is %d and my parent is %d\n", getpid(), getppid());
    return 1;
  }

  if(val > (*node).value)
  {
    return search_node((*node).right_son, val);
  }

  if(val < (*node).value))
  {
    return search_node((*node).left_son, val);
  }
}

void kill(node_t* node, int val)
{
  if(node == NULL)
    return;

  if((*node).value == val)
  {

  }
}
