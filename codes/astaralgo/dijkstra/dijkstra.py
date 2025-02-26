import pygame
import dijkstra
import math

pygame.init()

# Screen setup
WIDTH, HEIGHT = 800, 600
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Dijkstra's Algorithm Visualizer")
clock = pygame.time.Clock()
font = pygame.font.Font(None, 24)

# Data structures
nodes = []  # List of (x, y) coordinates for each node
edges = []  # List of (node1_idx, node2_idx, weight) tuples
start = None  # Index of start node
end = None  # Index of end node
state = "create_nodes"  # Possible states: create_nodes, create_edges, select_start, select_end, compute_path
edge_start = None  # Stores first node when creating an edge

def get_node_at_position(pos):
    """Returns index of the closest node if within radius, else None."""
    for i, node in enumerate(nodes):
        dx = pos[0] - node[0]
        dy = pos[1] - node[1]
        if dx * dx + dy * dy <= 25:  # Within 5 pixels radius (5^2 = 25)
            return i
    return None

def draw():
    """Renders nodes, edges, and UI text."""
    screen.fill((255, 255, 255))

    # Draw edges
    for edge in edges:
        n1, n2, weight = edge
        pygame.draw.line(screen, (0, 0, 0), nodes[n1], nodes[n2], 2)

    # Draw nodes
    for i, node in enumerate(nodes):
        if i == start:
            color = (0, 255, 0)  # Start node (Green)
        elif i == end:
            color = (255, 0, 0)  # End node (Red)
        else:
            color = (0, 0, 255)  # Default node (Blue)
        pygame.draw.circle(screen, color, node, 5)

    # Highlight first selected node in edge creation
    if state == "create_edges" and edge_start is not None:
        pygame.draw.circle(screen, (255, 165, 0), nodes[edge_start], 7, 2)  # Orange circle

    # Draw instructions
    instructions = {
        "create_nodes": "Left-click to create nodes. Press Enter when done.",
        "create_edges": "Right-click to create edges (select two nodes to connect). Press Enter when done.",
        "select_start": "Left-click to set a START node. Press Enter when done.",
        "select_end": "Left-click to set an END node. Press Enter to compute the shortest path.",
        "compute_path": "Shortest path computed and drawn in red. Press ESC to exit."
    }
    text_surface = font.render(instructions[state], True, (0, 0, 0))
    screen.blit(text_surface, (10, 10))

    pygame.display.update()

def find_shortest_path():
    """Runs Dijkstra's algorithm and returns the shortest path as a list of (x, y) points."""
    graph_nodes = [dijkstra.Node(x, y, 1.0) for (x, y) in nodes]
    graph_edges = []

    for n1, n2, weight in edges:
        graph_edges.append(dijkstra.Edge(graph_nodes[n1], graph_nodes[n2], weight))
        graph_edges.append(dijkstra.Edge(graph_nodes[n2], graph_nodes[n1], weight))  # Undirected

    graph = dijkstra.Graph(graph_nodes, graph_edges, graph_nodes[start], graph_nodes[end])
    path = graph.findShortestPath()

    return [(node.x, node.y) for node in path]

# Main event loop
running = True
while running:
    draw()
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

        elif event.type == pygame.MOUSEBUTTONDOWN:
            if event.button == 1:  # Left-click
                if state == "create_nodes":
                    nodes.append(event.pos)
                elif state == "select_start":
                    idx = get_node_at_position(event.pos)
                    if idx is not None:
                        start = idx
                elif state == "select_end":
                    idx = get_node_at_position(event.pos)
                    if idx is not None:
                        end = idx

            elif event.button == 3:  # Right-click
                if state == "create_edges":
                    idx = get_node_at_position(event.pos)
                    if idx is not None:
                        if edge_start is None:
                            edge_start = idx
                        else:
                            if edge_start != idx:
                                # Compute weight as Euclidean distance
                                x1, y1 = nodes[edge_start]
                                x2, y2 = nodes[idx]
                                weight = round(math.sqrt((x2 - x1) ** 2 + (y2 - y1) ** 2), 2)
                                edges.append((edge_start, idx, weight))
                            edge_start = None

        elif event.type == pygame.KEYDOWN:
            if event.key == pygame.K_RETURN:
                if state == "create_nodes":
                    if nodes:
                        state = "create_edges"
                    else:
                        print("Create at least one node first!")

                elif state == "create_edges":
                    state = "select_start"
                    edge_start = None

                elif state == "select_start":
                    if start is not None:
                        state = "select_end"
                    else:
                        print("Please select a start node first!")

                elif state == "select_end":
                    if end is not None:
                        state = "compute_path"
                        path = find_shortest_path()
                        # Draw shortest path in red
                        for i in range(len(path) - 1):
                            pygame.draw.line(screen, (255, 0, 0), path[i], path[i + 1], 3)
                        pygame.display.update()
                    else:
                        print("Please select an end node first!")

            elif event.key == pygame.K_ESCAPE:
                running = False

    clock.tick(60)

pygame.quit()
