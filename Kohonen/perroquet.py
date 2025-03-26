import numpy as np
from PIL import Image
import matplotlib.pyplot as plt

# exercice 3 : compression d'image
LEARNING_RATE = 0.2
dvp = 2
dvn = 3
MAX_ITERATIONS = 5  #

# Classe Neuron
class Neuron:
    def __init__(self, r, g, b):
        self.weights = np.array([r, g, b], dtype=float)

#PPM
def read_ppm(filename):
    try:
        img = Image.open(filename).convert("RGB")
        return np.array(img)
    except Exception as e:
        print(f"Erreur lors de la lecture du fichier PPM {filename} : {e}")
        return None

# Sauvegarde
def save_image(image_array, filename):
    img = Image.fromarray(image_array.astype('uint8'), 'RGB')
    img.save(filename)
    print(f"Image compressée sauvegardée sous le nom : {filename}")

# Initialiser les neurones
def initialise_neurons(nb_neurons):
    return [Neuron(np.random.uniform(0, 255), np.random.uniform(0, 255), np.random.uniform(0, 255)) for _ in range(nb_neurons)]

# neurone gagnant
def find_winner(neurons, pixel):
    potentials = [np.linalg.norm(neuron.weights - pixel) for neuron in neurons]
    return np.argmin(potentials)

# Mettre à jour des poids
def update_weights(neurons, winner_idx, pixel):
    for i, neuron in enumerate(neurons):
        distance = abs(i - winner_idx)
        if distance <= dvn:  
            if distance == 0:
                phi = 1
            elif distance <= dvp:
                phi = 0.5
            else:
                phi = 0  
            neuron.weights += LEARNING_RATE * phi * (np.array(pixel) - neuron.weights)

# Compresser l'image
def compress_image(image_data, neurons):
    compressed_image = np.zeros_like(image_data)
    for i in range(image_data.shape[0]):
        for j in range(image_data.shape[1]):
            pixel = image_data[i, j]
            winner_idx = find_winner(neurons, pixel)
            compressed_image[i, j] = neurons[winner_idx].weights
    return compressed_image


def compress_image_with_kohonen(image_data, nb_colors, max_iterations=5):
    neurons = initialise_neurons(nb_colors)
    
    for iteration in range(max_iterations):
        print(f"Compression {nb_colors} couleurs - Itération {iteration + 1}/{max_iterations}")
        for i in range(image_data.shape[0]):
            for j in range(image_data.shape[1]):
                pixel = image_data[i, j]
                winner_idx = find_winner(neurons, pixel)
                update_weights(neurons, winner_idx, pixel)
    
    #
    compressed_image = compress_image(image_data, neurons)
    # S'assurer que les valeurs sont dans [0,255]
    compressed_image = np.clip(compressed_image, 0, 255).astype(np.uint8)
    return compressed_image

# main
def main():
    image_data = read_ppm("perroquet.ppm")
    if image_data is None:
        return
    print(f"Image de taille {image_data.shape[0]}x{image_data.shape[1]} lue avec succès.")
    
    # 3 colors
    color_counts = [16, 32, 256]
    compressed_images = []
    
    for nb_colors in color_counts:
        compressed = compress_image_with_kohonen(image_data, nb_colors, max_iterations=MAX_ITERATIONS)
        compressed_images.append(compressed)
        save_image(compressed, f"compressend_{nb_colors}.ppm")
    
    # images
    fig, axes = plt.subplots(1, len(color_counts) + 1, figsize=(18, 6))
    
    # Image originale
    axes[0].imshow(image_data)
    axes[0].set_title("Originale")
    axes[0].axis("off")
    
    for idx, nb_colors in enumerate(color_counts):
        axes[idx + 1].imshow(compressed_images[idx])
        axes[idx + 1].set_title(f"Compressée {nb_colors} couleurs")
        axes[idx + 1].axis("off")
    
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    main()
