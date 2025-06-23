import tkinter as tk
from PIL import Image, ImageDraw
import numpy as np
import pandas as pd
from neural_net import NeuralNetwork

# ————————————— Data Loading & Preprocessing —————————————

def load_and_preprocess(train_path="train.csv"):
    """
    Loads train.csv, normalizes pixel values to [0,1],
    returns X (N×784 float64) and Y one-hot (N×10 float64).
    """
    df = pd.read_csv(train_path)
    labels = df.pop("label").values.astype(np.int64)
    X = df.values.astype(np.float64) / 255.0
    # One-hot encode
    N = labels.shape[0]
    Y = np.zeros((N, 10), dtype=np.float64)
    Y[np.arange(N), labels] = 1.0
    return X, Y

def train_val_split(X, Y, val_frac=0.1, seed=42):
    """Shuffles and splits into train/validation sets."""
    np.random.seed(seed)
    perm = np.random.permutation(len(X))
    cut = int(len(X) * val_frac)
    idx_val = perm[:cut]
    idx_train = perm[cut:]
    return X[idx_train], Y[idx_train], X[idx_val], Y[idx_val]

# ————————————— GUI Application —————————————

class DrawAndPredictApp:
    def __init__(self, model):
        self.model = model
        self.canvas_size = 280

        self.root = tk.Tk()
        self.root.title("Draw a Digit")

        self.canvas = tk.Canvas(self.root, width=self.canvas_size, height=self.canvas_size, bg='white')
        self.canvas.pack(padx=10, pady=10)
        self.canvas.bind("<B1-Motion>", self.draw)

        btn_frame = tk.Frame(self.root)
        btn_frame.pack()
        tk.Button(btn_frame, text="Predict", command=self.predict).grid(row=0, column=0, padx=5)
        tk.Button(btn_frame, text="Clear",   command=self.clear_canvas).grid(row=0, column=1, padx=5)

        self.label = tk.Label(self.root, text="Draw a digit and click Predict", font=("Helvetica", 16))
        self.label.pack(pady=10)

        # Mirror canvas into a PIL image
        self.image = Image.new("L", (self.canvas_size, self.canvas_size), color=255)
        self.draw_ctx = ImageDraw.Draw(self.image)

        # Choose resampling filter
        if hasattr(Image, 'Resampling'):
            self.resample = Image.Resampling.LANCZOS
        else:
            self.resample = Image.LANCZOS

    def draw(self, event):
        x, y, r = event.x, event.y, 10
        self.canvas.create_oval(x-r, y-r, x+r, y+r, fill='black', outline='black')
        self.draw_ctx.ellipse([x-r, y-r, x+r, y+r], fill=0)

    def clear_canvas(self):
        self.canvas.delete("all")
        self.draw_ctx.rectangle([0, 0, self.canvas_size, self.canvas_size], fill=255)
        self.label.config(text="Draw a digit and click Predict")

    def predict(self):
        # Resize → 28×28, invert & normalize
        img28 = self.image.resize((28,28), self.resample)
        arr = 255.0 - np.array(img28, dtype=np.float64)
        arr /= 255.0
        x = arr.flatten()  # shape (784,)
        y_pred = self.model.forward(x)  # length-10 array
        digit = int(np.argmax(y_pred))
        self.label.config(text=f"Prediction: {digit}")

    def run(self):
        self.root.mainloop()

# ————————————— Main Training + GUI —————————————

def main():
    # 1) Load data
    print("Loading and preprocessing training data...")
    X, Y = load_and_preprocess("train.csv")

    # 2) Split into train/validation
    X_train, Y_train, X_val, Y_val = train_val_split(X, Y, val_frac=0.1)
    print(f"Training on {len(X_train)} samples, validating on {len(X_val)} samples")

    # 3) Create C++ NeuralNetwork
    nn = NeuralNetwork(in_dim=784, hidden_layers=2, hidden_size=128, out_dim=10)

    # 4) Train for a few epochs
    epochs = 10
    batch_size = 32
    lr = 0.15

    for epoch in range(1, epochs+1):
        # shuffle each epoch
        perm = np.random.permutation(len(X_train))
        X_shuf, Y_shuf = X_train[perm], Y_train[perm]

        # mini-batch updates
        for i in range(0, len(X_shuf), batch_size):
            X_batch = X_shuf[i:i+batch_size]
            Y_batch = Y_shuf[i:i+batch_size]
            nn.train_batch(X_batch, Y_batch, batch_size=X_batch.shape[0], lr=lr)

        # compute validation accuracy
        preds = np.vstack([nn.forward(x) for x in X_val])
        acc = np.mean(np.argmax(preds,1) == np.argmax(Y_val,1))
        print(f"Epoch {epoch}/{epochs} — val accuracy: {acc:.4f}")

    # 5) Launch drawing GUI with the trained model
    print("Training complete; launching drawing app.")
    app = DrawAndPredictApp(nn)
    app.run()

if __name__ == "__main__":
    main()
