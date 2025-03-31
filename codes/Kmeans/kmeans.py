import matplotlib.pyplot as plt
import numpy as np
import kmeans
import webcolors

# Example data points (commented out)
'''
data = [
    [1.0, 2.0], [1.5, 1.8], [5.0, 8.0],
    [8.0, 8.0], [1.0, 0.6], [9.0, 11.0],
    [8.0, 2.0], [10.0, 2.0], [9.0, 3.0],
    [4.0, 7.0], [5.0, 6.0], [3.0, 7.0]
]
'''

# Generate random data
num_points = 500
dimensions = 3  # Set dimension to 3 (can be 1, 2, or 3)
data = np.random.rand(num_points, dimensions) * (num_points / 15)
data = data.tolist()

# Run K-means from C++ module
k = 15
iterations = int(np.floor(( num_points ** 2.5 ) / 2))
clusters = kmeans.kmeans(dimensions, data, k, iterations)

# Hardcoded CSS3 colors dictionary (hex -> color name)
CSS3_HEX_TO_NAMES = {
    '#F0F8FF': 'aliceblue',
    '#FAEBD7': 'antiquewhite',
    '#00FFFF': 'aqua',
    '#7FFFD4': 'aquamarine',
    '#F0FFFF': 'azure',
    '#F5F5DC': 'beige',
    '#FFE4C4': 'bisque',
    '#000000': 'black',
    '#FFEBCD': 'blanchedalmond',
    '#0000FF': 'blue',
    '#8A2BE2': 'blueviolet',
    '#A52A2A': 'brown',
    '#DEB887': 'burlywood',
    '#5F9EA0': 'cadetblue',
    '#7FFF00': 'chartreuse',
    '#D2691E': 'chocolate',
    '#FF7F50': 'coral',
    '#6495ED': 'cornflowerblue',
    '#FFF8DC': 'cornsilk',
    '#DC143C': 'crimson',
    '#00FFFF': 'cyan',
    '#00008B': 'darkblue',
    '#008B8B': 'darkcyan',
    '#B8860B': 'darkgoldenrod',
    '#A9A9A9': 'darkgray',
    '#006400': 'darkgreen',
    '#BDB76B': 'darkkhaki',
    '#8B008B': 'darkmagenta',
    '#556B2F': 'darkolivegreen',
    '#FF8C00': 'darkorange',
    '#9932CC': 'darkorchid',
    '#8B0000': 'darkred',
    '#E9967A': 'darksalmon',
    '#8FBC8F': 'darkseagreen',
    '#483D8B': 'darkslateblue',
    '#2F4F4F': 'darkslategray',
    '#00CED1': 'darkturquoise',
    '#9400D3': 'darkviolet',
    '#FF1493': 'deeppink',
    '#00BFFF': 'deepskyblue',
    '#696969': 'dimgray',
    '#1E90FF': 'dodgerblue',
    '#B22222': 'firebrick',
    '#FFFAF0': 'floralwhite',
    '#228B22': 'forestgreen',
    '#FF00FF': 'fuchsia',
    '#DCDCDC': 'gainsboro',
    '#F8F8FF': 'ghostwhite',
    '#FFD700': 'gold',
    '#DAA520': 'goldenrod',
    '#808080': 'gray',
    '#008000': 'green',
    '#ADFF2F': 'greenyellow',
    '#F0FFF0': 'honeydew',
    '#FF69B4': 'hotpink',
    '#CD5C5C': 'indianred',
    '#4B0082': 'indigo',
    '#FFFFF0': 'ivory',
    '#F0E68C': 'khaki',
    '#E6E6FA': 'lavender',
    '#FFF0F5': 'lavenderblush',
    '#7CFC00': 'lawngreen',
    '#FFFACD': 'lemonchiffon',
    '#ADD8E6': 'lightblue',
    '#F08080': 'lightcoral',
    '#E0FFFF': 'lightcyan',
    '#FAFAD2': 'lightgoldenrodyellow',
    '#D3D3D3': 'lightgray',
    '#90EE90': 'lightgreen',
    '#FFB6C1': 'lightpink',
    '#FFA07A': 'lightsalmon',
    '#20B2AA': 'lightseagreen',
    '#87CEFA': 'lightskyblue',
    '#778899': 'lightslategray',
    '#B0C4DE': 'lightsteelblue',
    '#FFFFE0': 'lightyellow',
    '#00FF00': 'lime',
    '#32CD32': 'limegreen',
    '#FAF0E6': 'linen',
    '#FF00FF': 'magenta',
    '#800000': 'maroon',
    '#66CDAA': 'mediumaquamarine',
    '#0000CD': 'mediumblue',
    '#BA55D3': 'mediumorchid',
    '#9370DB': 'mediumpurple',
    '#3CB371': 'mediumseagreen',
    '#7B68EE': 'mediumslateblue',
    '#00FA9A': 'mediumspringgreen',
    '#48D1CC': 'mediumturquoise',
    '#C71585': 'mediumvioletred',
    '#191970': 'midnightblue',
    '#F5FFFA': 'mintcream',
    '#FFE4E1': 'mistyrose',
    '#FFE4B5': 'moccasin',
    '#FFDEAD': 'navajowhite',
    '#000080': 'navy',
    '#FDF5E6': 'oldlace',
    '#808000': 'olive',
    '#6B8E23': 'olivedrab',
    '#FFA500': 'orange',
    '#FF4500': 'orangered',
    '#DA70D6': 'orchid',
    '#EEE8AA': 'palegoldenrod',
    '#98FB98': 'palegreen',
    '#AFEEEE': 'paleturquoise',
    '#DB7093': 'palevioletred',
    '#FFEFD5': 'papayawhip',
    '#FFDAB9': 'peachpuff',
    '#CD853F': 'peru',
    '#FFC0CB': 'pink',
    '#DDA0DD': 'plum',
    '#B0E0E6': 'powderblue',
    '#800080': 'purple',
    '#FF0000': 'red',
    '#BC8F8F': 'rosybrown',
    '#4169E1': 'royalblue',
    '#8B4513': 'saddlebrown',
    '#FA8072': 'salmon',
    '#F4A460': 'sandybrown',
    '#2E8B57': 'seagreen',
    '#FFF5EE': 'seashell',
    '#A0522D': 'sienna',
    '#C0C0C0': 'silver',
    '#87CEEB': 'skyblue',
    '#6A5ACD': 'slateblue',
    '#708090': 'slategray',
    '#FFFAFA': 'snow',
    '#00FF7F': 'springgreen',
    '#4682B4': 'steelblue',
    '#D2B48C': 'tan',
    '#008080': 'teal',
    '#D8BFD8': 'thistle',
    '#FF6347': 'tomato',
    '#40E0D0': 'turquoise',
    '#EE82EE': 'violet',
    '#F5DEB3': 'wheat',
    '#FFFFFF': 'white',
    '#F5F5F5': 'whitesmoke',
    '#FFFF00': 'yellow',
    '#9ACD32': 'yellowgreen'
}

def closest_color(rgb):
    """Find the closest human-readable color name to the given RGB tuple (0-255)."""
    min_distance = float('inf')
    closest_name = None
    for hex_value, name in CSS3_HEX_TO_NAMES.items():
        r_c, g_c, b_c = webcolors.hex_to_rgb(hex_value)
        distance = sum((comp1 - comp2) ** 2 for comp1, comp2 in zip(rgb, (r_c, g_c, b_c)))
        if distance < min_distance:
            min_distance = distance
            closest_name = name
    return closest_name

# Functions for plotting in 1D, 2D, and 3D
def plot_1d(clusters):
    """Plot clusters in 1D."""
    plt.figure(figsize=(12, 6))
    colors = plt.cm.rainbow(np.linspace(0, 1, len(clusters)))
    for idx, cluster in enumerate(clusters):
        if len(cluster) > 0:
            cluster = np.array(cluster)
            plt.scatter(cluster[:, 0], np.zeros_like(cluster[:, 0]), color=colors[idx], label=f'Cluster {idx + 1}')
    plt.xlabel('X')
    plt.ylabel('Cluster')
    plt.title('K-means Clustering in 1D using Pybind11')
    plt.legend()
    plt.grid(True)
    plt.show()

def plot_2d(clusters):
    """Plot clusters in 2D."""
    plt.figure(figsize=(12, 10))
    colors = plt.cm.rainbow(np.linspace(0, 1, len(clusters)))
    for idx, cluster in enumerate(clusters):
        if len(cluster) > 0:
            cluster = np.array(cluster)
            plt.scatter(cluster[:, 0], cluster[:, 1], color=colors[idx], label=f'Cluster {idx + 1}')
    plt.xlabel('X')
    plt.ylabel('Y')
    plt.title('K-means Clustering in 2D using Pybind11')
    plt.legend()
    plt.grid(True)
    plt.show()

def plot_3d(clusters):
    """Plot clusters in 3D."""
    fig = plt.figure(figsize=(12, 10))
    ax = fig.add_subplot(111, projection='3d')
    colors = plt.cm.rainbow(np.linspace(0, 1, len(clusters)))
    for idx, cluster in enumerate(clusters):
        if len(cluster) > 0:
            cluster = np.array(cluster)
            ax.scatter(cluster[:, 0], cluster[:, 1], cluster[:, 2], color=colors[idx], label=f'Cluster {idx + 1}')
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    ax.set_title('K-means Clustering in 3D using Pybind11')
    ax.legend()
    plt.show()

def check_cluster(point):
    """Check which cluster the point belongs to and print the cluster and its human-readable color."""
    colors = plt.cm.rainbow(np.linspace(0, 1, len(clusters)))
    closest_cluster = -1
    min_distance = float('inf')
    for idx, cluster in enumerate(clusters):
        cluster_array = np.array(cluster)
        if len(cluster_array) > 0:
            distances = np.linalg.norm(cluster_array - np.array(point), axis=1)
            closest_point_distance = np.min(distances)
            if closest_point_distance < min_distance:
                min_distance = closest_point_distance
                closest_cluster = idx
    if closest_cluster != -1:
        # Get the RGB color (0-1 range), convert to 0-255 integers
        color_rgb = (colors[closest_cluster][:3] * 255).astype(int)
        color_name = closest_color(tuple(color_rgb))
        print(f"The point {point} is in Cluster {closest_cluster + 1} colored in {color_name}.")
    else:
        print(f"The point {point} does not belong to any cluster.")

# Example: Check which cluster a point belongs to
# example_point = [3.5, 2.0, 1.5]  # Adjust dimension as needed
# check_cluster(example_point)

# Plot the clusters based on dimension
if dimensions == 1:
    plot_1d(clusters)
elif dimensions == 2:
    plot_2d(clusters)
elif dimensions == 3:
    plot_3d(clusters)
