#!/usr/bin/env python3
"""Affiche un graphe avant/après un partitionnement MPI.

Usage :
    python3 pytools/visualiser_partition.py fichierMonde/monde1.txt resulatsPartition/resultat_partition.txt
    python3 pytools/visualiser_partition.py fichierMonde/monde_grand.txt resulatsPartition/resultat_partition.txt --output images/comparaison.png
"""

import argparse
import re
from pathlib import Path

import matplotlib


def lire_monde(chemin):
    import networkx as nx

    lignes = chemin.read_text(encoding="utf-8").splitlines()
    lignes = [ligne.strip() for ligne in lignes if ligne.strip()]

    n = int(lignes[0])
    graphe = nx.Graph()

    for ligne in lignes[1 : n + 1]:
        identifiant, nom, population = ligne.split()
        graphe.add_node(int(identifiant), nom=nom, population=int(population))

    nb_ponts = int(lignes[n + 1])
    ponts = lignes[n + 2 : n + 2 + nb_ponts]
    if len(ponts) != nb_ponts:
        raise ValueError("Nombre de ponts incohérent dans le fichier du monde.")

    for ligne in ponts:
        a, b = map(int, ligne.split())
        if a not in graphe or b not in graphe:
            raise ValueError(f"Pont invalide : {a} {b} (île inexistante).")
        graphe.add_edge(a, b)

    return graphe


def lire_partitions(chemin, graphe):
    """Lit les lignes 'Partition 0 : 1 5 7' imprimées par le main C++."""
    texte = chemin.read_text(encoding="utf-8")
    affectations = {}
    partitions = {}

    for ligne in texte.splitlines():
        match = re.match(r"^\s*Partition\s+(\d+)\s*:\s*(.*?)\s*$", ligne)
        if not match:
            continue

        processus = int(match.group(1))
        if processus in partitions:
            raise ValueError(f"La partition {processus} apparaît plusieurs fois.")

        sommets = [int(x) for x in match.group(2).split()]
        partitions[processus] = sommets

        for sommet in sommets:
            if sommet not in graphe:
                raise ValueError(f"Sommet {sommet} absent du monde.")
            if sommet in affectations:
                raise ValueError(f"Sommet {sommet} attribué plusieurs fois.")
            affectations[sommet] = processus

    if not partitions:
        raise ValueError("Aucune ligne 'Partition P : ...' trouvée dans le résultat C++.")

    manquants = set(graphe.nodes) - set(affectations)
    if manquants:
        raise ValueError(f"{len(manquants)} sommets non attribués (exemple : {sorted(manquants)[:8]}).")

    return affectations, partitions


def visualiser(graphe, affectations, partitions, sortie, afficher=False):
    import matplotlib.pyplot as plt
    import networkx as nx

    n = graphe.number_of_nodes()
    # Une seule disposition, réutilisée à l'identique dans les deux graphes.
    positions = nx.spring_layout(graphe, seed=42, iterations=35)

    fig, axes = plt.subplots(1, 2, figsize=(19, 9), constrained_layout=True)
    taille_noeud = 300 if n <= 30 else (70 if n <= 200 else 12)
    epaisseur = 0.7 if n <= 200 else 0.25
    etiquettes = n <= 35

    nx.draw_networkx_edges(graphe, positions, ax=axes[0], width=epaisseur, alpha=0.25)
    nx.draw_networkx_nodes(
        graphe, positions, ax=axes[0], node_size=taille_noeud,
        node_color="#8192a6", linewidths=0,
    )
    if etiquettes:
        nx.draw_networkx_labels(graphe, positions, ax=axes[0], font_size=8)

    nb_coupes = sum(affectations[a] != affectations[b] for a, b in graphe.edges())
    internes = [(a, b) for a, b in graphe.edges() if affectations[a] == affectations[b]]
    coupes = [(a, b) for a, b in graphe.edges() if affectations[a] != affectations[b]]

    nx.draw_networkx_edges(
        graphe, positions, edgelist=internes,
        ax=axes[1], width=epaisseur, alpha=0.22, edge_color="#556477",
    )
    nx.draw_networkx_edges(
        graphe, positions, edgelist=coupes,
        ax=axes[1], width=epaisseur + 0.3, alpha=0.48, edge_color="#d64545",
    )

    couleurs = plt.get_cmap("tab20", max(1, len(partitions)))
    for index, proc in enumerate(sorted(partitions)):
        sommets = partitions[proc]
        charge = sum(graphe.nodes[v]["population"] for v in sommets)
        nx.draw_networkx_nodes(
            graphe, positions, nodelist=sommets, ax=axes[1],
            node_size=taille_noeud, node_color=[couleurs(index)],
            linewidths=0,
            label=f"P{proc} : {len(sommets)} îles / {charge} hab.",
        )

    if etiquettes:
        nx.draw_networkx_labels(graphe, positions, ax=axes[1], font_size=8)

    axes[0].set_title(f"AVANT — {n} îles, {graphe.number_of_edges()} ponts", fontsize=14)
    axes[1].set_title(
        f"APRÈS — {len(partitions)} partitions MPI, {nb_coupes} ponts coupés",
        fontsize=14,
    )
    for ax in axes:
        ax.set_axis_off()
        ax.set_aspect("equal")

    if len(partitions) <= 20:
        axes[1].legend(loc="lower center", bbox_to_anchor=(0.5, -0.09),
                       ncol=min(4, len(partitions)), fontsize=8, frameon=False)

    sortie.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(sortie, dpi=170, bbox_inches="tight")
    print(f"Image enregistrée : {sortie}")
    print(f"Ponts coupés : {nb_coupes} / {graphe.number_of_edges()}")
    if afficher:
        plt.show()
    plt.close(fig)


def main():
    parser = argparse.ArgumentParser(description="Compare le graphe avant/après partitionnement MPI.")
    parser.add_argument("monde", type=Path, help="Fichier texte décrivant le monde")
    parser.add_argument("resultat", type=Path, help="Sortie texte du main_test_partition.cpp")
    parser.add_argument("--output", type=Path, default=Path("images/comparaison_partition.png"),
                        help="Image produite (défaut : images/comparaison_partition.png)")
    parser.add_argument("--show", action="store_true", help="Ouvrir une fenêtre Matplotlib")
    args = parser.parse_args()

    if not args.show:
        matplotlib.use("Agg")

    graphe = lire_monde(args.monde)
    affectations, partitions = lire_partitions(args.resultat, graphe)
    visualiser(graphe, affectations, partitions, args.output, args.show)


if __name__ == "__main__":
    main()
