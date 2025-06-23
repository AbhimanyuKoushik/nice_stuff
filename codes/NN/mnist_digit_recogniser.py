import numpy as np
import pandas as pd
from neural_net import NeuralNetwork

def load_data(train_path="train.csv", test_path="test.csv"):
    # Train: first column is label, rest are 784 pixels
    df_train = pd.read_csv(train_path)
    y = df_train.pop("label").values.astype(np.float64)
    X = df_train.values.astype(np.float64) / 255.0  # normalize to [0,1]

    # Test: just pixels
    df_test = pd.read_csv(test_path)
    X_test = df_test.values.astype(np.float64) / 255.0

    return X, y, X_test

def one_hot_encode(y, num_classes=10):
    M = y.shape[0]
    Y = np.zeros((M, num_classes), dtype=np.float64)
    Y[np.arange(M), y.astype(int)] = 1.0
    return Y

def train_validation_split(X, Y, val_frac=0.1, seed=42):
    np.random.seed(seed)
    idx = np.random.permutation(len(X))
    n_val = int(len(X) * val_frac)
    val_idx, train_idx = idx[:n_val], idx[n_val:]
    return X[train_idx], Y[train_idx], X[val_idx], Y[val_idx]

def main():
    # 1) Load & preprocess
    print("Loading data...")
    X, y, X_test = load_data()
    Y = one_hot_encode(y)

    # 2) Train/validation split
    X_train, Y_train, X_val, Y_val = train_validation_split(X, Y, val_frac=0.1)
    print(f"Training on {len(X_train)} samples, validating on {len(X_val)} samples")

    # 3) Create a network: 784 inputs → two hidden layers (128,64) → 10 outputs
    nn = NeuralNetwork(in_dim=784,
                       hidden_layers=2,
                       hidden_size=128,
                       out_dim=10)

    # 4) Training loop
    epochs = 20
    batch_size = 64
    lr = 0.1

    for epoch in range(1, epochs + 1):
        # shuffle
        perm = np.random.permutation(len(X_train))
        X_shuf = X_train[perm]
        Y_shuf = Y_train[perm]

        # mini-batch
        for i in range(0, len(X_shuf), batch_size):
            X_batch = X_shuf[i:i+batch_size]
            Y_batch = Y_shuf[i:i+batch_size]
            nn.train_batch(X_batch, Y_batch, batch_size=X_batch.shape[0], lr=lr)

        # compute validation accuracy
        preds = np.vstack([nn.forward(x) for x in X_val])
        pred_labels = np.argmax(preds, axis=1)
        true_labels = np.argmax(Y_val, axis=1)
        acc = np.mean(pred_labels == true_labels)
        print(f"Epoch {epoch:2d}/{epochs} — val accuracy: {acc:.4f}")

    # 5) Predict on test set
    print("Predicting on test set...")
    test_preds = np.vstack([nn.forward(x) for x in X_test])
    test_labels = np.argmax(test_preds, axis=1).astype(int)

    # 6) Write submission.csv
    submission = pd.DataFrame({
        "ImageId": np.arange(1, len(test_labels) + 1),
        "Label": test_labels
    })
    submission.to_csv("submission.csv", index=False)
    print("Wrote submission.csv")

if __name__ == "__main__":
    main()
