document.addEventListener('DOMContentLoaded', () => {
    const container = document.getElementById('threejs-plot');
    if (!container) return;

    // Parse cluster data passed from the Flask server
    const plotData = JSON.parse(container.dataset.plot);

    // Scene setup
    const scene = new THREE.Scene();
    scene.background = new THREE.Color(0x121212);

    const camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);
    camera.position.set(15, 15, 15);

    const renderer = new THREE.WebGLRenderer({ antialias: true });
    renderer.setSize(window.innerWidth * 0.8, window.innerHeight * 0.8);
    container.appendChild(renderer.domElement);

    // Add OrbitControls for interactivity
    const controls = new THREE.OrbitControls(camera, renderer.domElement);
    controls.enableDamping = true;
    controls.dampingFactor = 0.1;

    // Create clusters using viridis color mapping
    plotData.forEach((cluster, index) => {
        const hue = index * 360 / plotData.length;
        const color = new THREE.Color(`hsl(${hue}, 70%, 50%)`);
        const geometry = new THREE.BufferGeometry();
        const vertices = [];
        for (let i = 0; i < cluster.x.length; i++) {
            vertices.push(cluster.x[i], cluster.y[i], cluster.z[i]);
        }
        geometry.setAttribute('position', new THREE.Float32BufferAttribute(vertices, 3));
        const material = new THREE.PointsMaterial({ color: color, size: 0.3 });
        const points = new THREE.Points(geometry, material);
        scene.add(points);
    });

    // Add simple axis lines (X in red, Y in green, Z in blue)
    const axisLength = 10;
    const axesHelper = new THREE.AxesHelper(axisLength);
    scene.add(axesHelper);

    // Optionally add text labels for axes using built-in canvas textures:
    function createTextSprite(message, parameters) {
        if (parameters === undefined) parameters = {};
        const fontface = parameters.hasOwnProperty('fontface') ? parameters['fontface'] : 'Arial';
        const fontsize = parameters.hasOwnProperty('fontsize') ? parameters['fontsize'] : 24;
        const borderThickness = parameters.hasOwnProperty('borderThickness') ? parameters['borderThickness'] : 4;
        const borderColor = parameters.hasOwnProperty('borderColor') ? parameters['borderColor'] : { r:0, g:0, b:0, a:1.0 };
        const backgroundColor = parameters.hasOwnProperty('backgroundColor') ? parameters['backgroundColor'] : { r:255, g:255, b:255, a:1.0 };

        const canvas = document.createElement('canvas');
        const context = canvas.getContext('2d');
        context.font = fontsize + 'px ' + fontface;
        // get size data (height depends only on font size)
        const metrics = context.measureText(message);
        const textWidth = metrics.width;
        context.fillStyle = `rgba(${backgroundColor.r}, ${backgroundColor.g}, ${backgroundColor.b}, ${backgroundColor.a})`;
        context.strokeStyle = `rgba(${borderColor.r}, ${borderColor.g}, ${borderColor.b}, ${borderColor.a})`;
        context.lineWidth = borderThickness;
        // Draw background rectangle
        context.fillRect(borderThickness/2, borderThickness/2, textWidth + borderThickness, fontsize * 1.2 + borderThickness);
        // text color
        context.fillStyle = 'rgba(0, 0, 0, 1.0)';
        context.fillText(message, borderThickness, fontsize + borderThickness);
        // canvas contents will be used for a texture\n        const texture = new THREE.CanvasTexture(canvas);
        texture.needsUpdate = true;
        const spriteMaterial = new THREE.SpriteMaterial({ map: texture });
        const sprite = new THREE.Sprite(spriteMaterial);
        sprite.scale.set(4, 2, 1.0);
        return sprite;
    }

    // Add labels for X, Y, Z
    const labelX = createTextSprite("X", { fontsize: 32, borderThickness: 2, backgroundColor: {r:255, g:255, b:255, a:0.8} });
    labelX.position.set(axisLength, 0, 0);
    scene.add(labelX);
    
    const labelY = createTextSprite("Y", { fontsize: 32, borderThickness: 2, backgroundColor: {r:255, g:255, b:255, a:0.8} });
    labelY.position.set(0, axisLength, 0);
    scene.add(labelY);
    
    const labelZ = createTextSprite("Z", { fontsize: 32, borderThickness: 2, backgroundColor: {r:255, g:255, b:255, a:0.8} });
    labelZ.position.set(0, 0, axisLength);
    scene.add(labelZ);

    // Render loop
    function animate() {
        requestAnimationFrame(animate);
        controls.update();
        renderer.render(scene, camera);
    }
    animate();

    window.addEventListener('resize', () => {
        camera.aspect = window.innerWidth / window.innerHeight;
        camera.updateProjectionMatrix();
        renderer.setSize(window.innerWidth * 0.8, window.innerHeight * 0.8);
    });
})