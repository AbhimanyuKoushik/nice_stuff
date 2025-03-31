from flask import Flask, request, render_template, url_for
import numpy as np
import kmeans  # Your Pybind11 kmeans module
import matplotlib
matplotlib.use('Agg')  # Use non-GUI backend for speed
import matplotlib.pyplot as plt
from io import BytesIO
import base64
import math

# For interactive 3D plotting
import plotly.offline as pyo
import plotly.graph_objs as go

app = Flask(__name__)

# In-memory cache to store generated plot outputs keyed by (num_points, dimensions, clusters)
plot_cache = {}

def generate_plot(num_points, dimensions, clusters):
    key = (num_points, dimensions, clusters)
    if key in plot_cache:
        return plot_cache[key]

    # Generate random data
    data = np.random.rand(num_points, dimensions) * (num_points / 15)
    data = data.tolist()
    iterations = int(math.floor((num_points ** 2.5) / 2))
    
    # Run K-means clustering from the C++ module
    kmeans_clusters = kmeans.kmeans(dimensions, data, clusters, iterations)
    
    if dimensions == 3:
        # Create interactive 3D plot using Plotly with a darker grey background
        fig = go.Figure()
        num_clusters = len(kmeans_clusters)
        # Generate a list of colors (using HSL for evenly spaced colors)
        colors = [f"hsl({int(360 * i / num_clusters)}, 100%, 50%)" for i in range(num_clusters)]
        for idx, cluster in enumerate(kmeans_clusters):
            if cluster:
                cluster = np.array(cluster)
                trace = go.Scatter3d(
                    x=cluster[:, 0].tolist(),
                    y=cluster[:, 1].tolist(),
                    z=cluster[:, 2].tolist(),
                    mode='markers',
                    marker=dict(color=colors[idx], size=3),  # Marker size is 3
                    name=f'Cluster {idx + 1}'
                )
                fig.add_trace(trace)
        fig.update_layout(
            title='K-means Clustering in 3D',
            template='plotly_white',
            paper_bgcolor='#e0e0e0',  # Darker grey background for the paper
            plot_bgcolor='#e0e0e0',   # Darker grey background for the plot area
            scene=dict(
                xaxis_title='X',
                yaxis_title='Y',
                zaxis_title='Z',
                xaxis=dict(backgroundcolor='#e0e0e0'),
                yaxis=dict(backgroundcolor='#e0e0e0'),
                zaxis=dict(backgroundcolor='#e0e0e0')
            )
        )
        # Generate the interactive plot as an HTML div string
        plot_html = pyo.plot(fig, output_type='div', include_plotlyjs='cdn')
        plot_cache[key] = plot_html
        return plot_html

    elif dimensions == 1:
        fig, ax = plt.subplots(figsize=(12, 6))
        colors = plt.cm.rainbow(np.linspace(0, 1, len(kmeans_clusters)))
        for idx, cluster in enumerate(kmeans_clusters):
            if cluster:
                cluster = np.array(cluster)
                ax.scatter(cluster[:, 0], np.zeros_like(cluster[:, 0]),
                           color=colors[idx], label=f'Cluster {idx + 1}')
        ax.set_xlabel('X')
        ax.set_ylabel('Cluster')
        ax.set_title('K-means Clustering in 1D')
        ax.legend()
        ax.grid(True)
        # Convert plot to PNG
        buf = BytesIO()
        plt.savefig(buf, format='png', bbox_inches='tight')
        buf.seek(0)
        image_png = buf.getvalue()
        buf.close()
        plt.close(fig)
        graphic = base64.b64encode(image_png).decode('utf-8')
        plot_cache[key] = graphic
        return graphic

    elif dimensions == 2:
        fig, ax = plt.subplots(figsize=(12, 10))
        colors = plt.cm.rainbow(np.linspace(0, 1, len(kmeans_clusters)))
        for idx, cluster in enumerate(kmeans_clusters):
            if cluster:
                cluster = np.array(cluster)
                ax.scatter(cluster[:, 0], cluster[:, 1],
                           color=colors[idx], label=f'Cluster {idx + 1}')
        ax.set_xlabel('X')
        ax.set_ylabel('Y')
        ax.set_title('K-means Clustering in 2D')
        ax.legend()
        ax.grid(True)
        # Convert plot to PNG
        buf = BytesIO()
        plt.savefig(buf, format='png', bbox_inches='tight')
        buf.seek(0)
        image_png = buf.getvalue()
        buf.close()
        plt.close(fig)
        graphic = base64.b64encode(image_png).decode('utf-8')
        plot_cache[key] = graphic
        return graphic

    else:
        # Unsupported dimension
        fig, ax = plt.subplots()
        ax.text(0.5, 0.5, "Unsupported dimension", ha='center', va='center')
        buf = BytesIO()
        plt.savefig(buf, format='png', bbox_inches='tight')
        buf.seek(0)
        image_png = buf.getvalue()
        buf.close()
        plt.close(fig)
        graphic = base64.b64encode(image_png).decode('utf-8')
        plot_cache[key] = graphic
        return graphic

@app.route('/', methods=['GET', 'POST'])
def index():
    # Default parameters
    num_points = 500
    dimensions = 3
    clusters = 15
    image_data = None
    interactive_plot = None

    if request.method == 'POST':
        try:
            num_points = int(request.form.get('num_points', 100))
            dimensions = int(request.form.get('dimensions', 3))
            clusters = int(request.form.get('clusters', 10))
        except ValueError:
            pass
        
        if dimensions == 3:
            interactive_plot = generate_plot(num_points, dimensions, clusters)
        else:
            image_data = generate_plot(num_points, dimensions, clusters)
    
    return render_template('index.html',
                           num_points=num_points,
                           dimensions=dimensions,
                           clusters=clusters,
                           image_data=image_data,
                           interactive_plot=interactive_plot)

if __name__ == '__main__':
    app.run(debug=True)
