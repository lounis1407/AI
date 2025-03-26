import numpy as np
from PIL import Image
import matplotlib.pyplot as plt

# --- Paramètres globaux ---
NB_NEURONS = 50  # Nombre de neurones
LEARNING_RATE = 0.1
SIGMA_INITIAL = 10  # Rayon initial pour la fonction de voisinage
SIGMA_DECAY = 2000  # Constante de décroissance pour le rayon
MAX_ITERATIONS = 10000

# =============================================================================
# Classes
# =============================================================================

class City:
    """
    Classe City pour représenter une ville avec un nom et des coordonnées (x, y).
    """
    def __init__(self, name, x, y):
        self.name = name
        self.x = x
        self.y = y

class Neuron:
    """
    Classe Neuron pour représenter un neurone avec des poids (coordinates).
    """
    def __init__(self, x, y):
        self.weights = np.array([x, y], dtype=float)

# =============================================================================
# Chargement des données
# =============================================================================

def load_cities(filename):
    """
    Charge la liste de villes depuis un fichier texte.
    """
    cities = []
    try:
        with open(filename, "r") as file:
            for line in file:
                name, x_str, y_str = line.strip().split()
                cities.append(City(name, int(x_str), int(y_str)))
        print("Villes chargées avec succès.")
    except FileNotFoundError:
        print(f"Erreur : le fichier '{filename}' est introuvable.")
    except ValueError:
        print("Erreur : format du fichier invalide. "
              "Assurez-vous que chaque ligne contient un nom et deux entiers.")
    return cities

def read_ppm(filename):
    """
    Lit une image PPM et la convertit en tableau numpy (RGB).
    Retourne None en cas d'erreur.
    """
    try:
        img = Image.open(filename).convert("RGB")
        return np.array(img)
    except Exception as e:
        print(f"Erreur lors de la lecture du fichier PPM {filename} : {e}")
        return None

# =============================================================================
# Initialisation des neurones
# =============================================================================

def initialize_neurons_circular_barycenter(cities, radius=200):
    """
    Initialise NB_NEURONS neurones répartis régulièrement sur un cercle
    dont le centre est le barycentre (centre de gravité) des villes.
    """
    if not cities:
        print("Aucune ville fournie, retour d'une liste de neurones vide.")
        return []

    # 1) Calculer le barycentre des villes
    bary_x = np.mean([city.x for city in cities])
    bary_y = np.mean([city.y for city in cities])

    # 2) Calculer le pas angulaire
    angle_step = 2 * np.pi / NB_NEURONS

    # 3) Créer la liste des neurones
    neurons = []
    for i in range(NB_NEURONS):
        angle = i * angle_step
        x = bary_x + radius * np.cos(angle)
        y = bary_y + radius * np.sin(angle)
        neurons.append(Neuron(x, y))

    return neurons

# =============================================================================
# Fonctions SOM
# =============================================================================

def find_winner(neurons, city):
    """
    Trouve l'index du neurone gagnant (le plus proche du city).
    """
    distances = [np.linalg.norm(neuron.weights - np.array([city.x, city.y]))
                 for neuron in neurons]
    return np.argmin(distances)

def neighborhood_function_cauchy(distance, sigma):
    """
    Fonction de voisinage de type Cauchy.
    """
    return 1 / (1 + (distance / sigma) ** 2)

def update_weights(neurons, winner_idx, city, sigma):
    """
    Met à jour les poids des neurones selon leur distance au neurone gagnant
    et une fonction de voisinage de type Cauchy.
    """
    for i, neuron in enumerate(neurons):
        distance = min(abs(i - winner_idx), NB_NEURONS - abs(i - winner_idx))
        phi = neighborhood_function_cauchy(distance, sigma)
        neuron.weights += LEARNING_RATE * phi * (np.array([city.x, city.y]) - neuron.weights)

# =============================================================================
# Visualisation
# =============================================================================

def visualize(image_data, cities, neurons, iteration):
    """
    Affiche la carte, les villes et les neurones, à l'itération donnée.
    """
    plt.figure(figsize=(10, 10))
    plt.imshow(image_data)
    plt.axis("off")

    # Afficher les villes (en rouge)
    x_coords = [city.x for city in cities]
    y_coords = [city.y for city in cities]
    plt.scatter(x_coords, y_coords, color="red", label="Villes")

    # Afficher les noms des villes
    for city in cities:
        plt.text(city.x, city.y, city.name, fontsize=8, ha="right", va="bottom")

    # Afficher les neurones (en bleu)
    neuron_coords = np.array([neuron.weights for neuron in neurons])
    plt.scatter(neuron_coords[:, 0], neuron_coords[:, 1], color="blue", label="Neurones")

    # Tracer la "route" reliant les neurones (en vert)
    plt.plot(neuron_coords[:, 0], neuron_coords[:, 1], color="green", label="Route")

    plt.title(f"Réseau de Kohonen - Itération {iteration}")
    plt.legend()
    plt.show()

# =============================================================================
# Algorithme principal
# =============================================================================

def main():
    # Charger les villes et l'image
    cities = load_cities("Villes_et_positions_dans_image.txt")
    image_data = read_ppm("carte_france_vierge.ppm")

    # Vérifier la validité des données
    if not cities:
        print("Aucune ville chargée, arrêt du programme.")
        return
    if image_data is None:
        print("Aucune image chargée, arrêt du programme.")
        return

    # Initialiser les neurones en cercle autour du barycentre
    neurons = initialize_neurons_circular_barycenter(cities, radius=250)

    # ---------------- Affichage à l'itération 0 ----------------
    visualize(image_data, cities, neurons, iteration=0)

    # Boucle d'entraînement
    for iteration in range(1, MAX_ITERATIONS + 1):
        # Décroissance du rayon sigma
        sigma = SIGMA_INITIAL * np.exp(-iteration / SIGMA_DECAY)

        for city in cities:
            winner_idx = find_winner(neurons, city)
            update_weights(neurons, winner_idx, city, sigma)

        # Affiche toutes les 1000 itérations
        if iteration % 1000 == 0:
            visualize(image_data, cities, neurons, iteration)

if __name__ == "__main__":
    main()
