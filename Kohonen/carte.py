import numpy as np
import matplotlib.pyplot as plt

# Initialisation des paramètres
NB_NEURONS = 30
NB_DATA = 20
INITIAL_LEARNING_RATE = 0.1
LEARNING_RATE_DECAY = 0.00005  # Taux de décroissance du learning rate réduit
DVP_INITIAL = 1  # Rayon de voisinage proche initial
DVN_INITIAL = 2  # Rayon de voisinage éloigné initial
CONVERGENCE_THRESHOLD = 1.0  # Ajusté pour correspondre à l'échelle des données (0 à 200)
MAX_ITERATIONS = 5000  # Nombre d'itérations augmenté
TORIC = False  # Mettre à True pour une carte torique (circulaire)
DATA_MIN = 0
DATA_MAX = 200

# Structures de données
class Data:
    def __init__(self, x, y):
        self.x = x
        self.y = y

class Neuron:
    def __init__(self, x, y):
        self.weights = np.array([x, y])

# Générer des données aléatoires
def initialise_data():
    return [Data(np.random.uniform(DATA_MIN, DATA_MAX), np.random.uniform(DATA_MIN, DATA_MAX)) for _ in range(NB_DATA)]

# Générer des neurones avec des poids aléatoires
def initialise_neurons():
    # Répartir les neurones uniformément sur l'espace des données
    step = (DATA_MAX - DATA_MIN) / NB_NEURONS
    return [Neuron(DATA_MIN + i * step, DATA_MIN + i * step) for i in range(NB_NEURONS)]

# Calcul du potentiel (distance euclidienne)
def calculate_potential(neuron, data):
    return np.linalg.norm(neuron.weights - np.array([data.x, data.y]))

# Calcul de la distance torique entre deux indices
def toric_distance(i, j):
    return min(abs(i - j), NB_NEURONS - abs(i - j))

# Mettre à jour les poids des neurones
def update_weights(neurons, winner_idx, data, learning_rate, dvp, dvn):
    for i, neuron in enumerate(neurons):
        # Calcul de la distance selon la topologie
        if TORIC:
            distance = toric_distance(i, winner_idx)
        else:
            distance = abs(i - winner_idx)
        
        if distance <= dvn:  # Neurones dans le voisinage défini
            if distance == 0:
                phi = 1.0  # Neurone gagnant
            elif distance <= dvp:
                phi = 0.5  # Voisin proche
            elif distance <= dvn:
                phi = 0.0  # Voisin éloigné sans influence négative
            else:
                phi = 0  # En dehors du voisinage, pas de mise à jour
            
            # Mise à jour des poids
            if phi != 0:
                neuron.weights += learning_rate * phi * (np.array([data.x, data.y]) - neuron.weights)
                
                # Contraindre les poids à rester dans les limites [DATA_MIN, DATA_MAX]
                if TORIC:
                    neuron.weights = neuron.weights % DATA_MAX  # Bouclage des positions
                else:
                    neuron.weights = np.clip(neuron.weights, DATA_MIN, DATA_MAX)

# Trouver le neurone gagnant
def find_winner(neurons, data):
    potentials = [calculate_potential(neuron, data) for neuron in neurons]
    return np.argmin(potentials)

# Critère basé sur les déplacements moyens des neurones
def has_converged(neurons, prev_positions, threshold=CONVERGENCE_THRESHOLD):
    current_positions = np.array([n.weights for n in neurons])
    if prev_positions is None:
        return False
    displacement = np.linalg.norm(current_positions - prev_positions, axis=1).mean()
    return displacement < threshold

# Visualisation des neurones et des données
def visualize(neurons, data, iteration):
    plt.figure(figsize=(12, 10))
    plt.xlim(DATA_MIN, DATA_MAX)
    plt.ylim(DATA_MIN, DATA_MAX)

    # Afficher les données
    plt.scatter([d.x for d in data], [d.y for d in data], c='red', label='Data')

    # Afficher les neurones
    neuron_x = [n.weights[0] for n in neurons]
    neuron_y = [n.weights[1] for n in neurons]
    plt.scatter(neuron_x, neuron_y, c='blue', label='Neurons')

    # Connecter les neurones
    plt.plot(neuron_x, neuron_y, c='green', label='Connections')

    plt.title(f"Iteration {iteration}")
    plt.legend()
    plt.grid(True)
    plt.show()

# Boucle principale
def main():
    data = initialise_data()
    neurons = initialise_neurons()
    prev_positions = None
    learning_rate = INITIAL_LEARNING_RATE
    dvp = DVP_INITIAL
    dvn = DVN_INITIAL

    for iteration in range(1, MAX_ITERATIONS + 1):
        # Mélanger les données pour un apprentissage en ligne aléatoire
        np.random.shuffle(data)

        for d in data:
            winner_idx = find_winner(neurons, d)
            update_weights(neurons, winner_idx, d, learning_rate, dvp, dvn)

        # Calculer le déplacement moyen pour le critère de convergence et log
        displacement = None
        if iteration % 10 == 0 or iteration == 1:
            current_positions = np.array([n.weights for n in neurons])
            if prev_positions is not None:
                displacement = np.linalg.norm(current_positions - prev_positions, axis=1).mean()
                print(f"Iteration {iteration}, Déplacement moyen: {displacement:.4f}")
            else:
                print(f"Iteration {iteration}, Initialisation des positions.")
            visualize(neurons, data, iteration)

        # Vérifiez la convergence
        if has_converged(neurons, prev_positions):
            print(f"Convergence atteinte à l'itération {iteration}")
            break

        # Mettre à jour les positions précédentes
        prev_positions = np.array([n.weights for n in neurons])

        # Diminuer le taux d'apprentissage
        learning_rate = INITIAL_LEARNING_RATE * np.exp(-LEARNING_RATE_DECAY * iteration)

    else:
        print("Nombre maximum d'itérations atteint sans convergence.")

    # Visualisation finale
    visualize(neurons, data, iteration)

if __name__ == "__main__":
    main()
