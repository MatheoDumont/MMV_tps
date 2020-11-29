# Rapport du TP 1 de MMV
Notre groupe est compose de Jean-Baptiste Loutfalla et Matheo Dumont.
  
Le shader : <https://www.shadertoy.com/view/ws3Bz2>

![Resultat](data/screenshot.png)

# Notre shader

## Formes

La fonction modelant le terrain est `float Implicit(in vec3 p)` *ligne 166*. 
### I. Terrain
Ligne 27.
Nous composons notre terrain de rigde noise et de noise.

### II. Disque
Disque Ligne 63.
Nous utilisons un disque comme vu en cours melange(Ligne 83) avec la formule du cours :
```cpp
vec2 blend(in vec2 a, in vec2 b)
{
    float new_alpha = (a.y + b.y);
    float new_height = (a.x*a.y + b.x*b.y) / new_alpha;
    return vec2(new_height, new_alpha);
}
```
pour obtenir une vallee et y placer un lac.

### III. Tunnel
Segment Ligne 139.
Difference Ligne 110. 
En utilisant des SDB, nous calculons la distance a un segment et faisons une difference avec le terrain,
pour creer un tunnel qui relie le lac en premier plan a un autre.

### IV. Avancement et Zone supprimee dans la montagne
BlendImplicit Ligne 122. 
Sphere Ligne 128.
Nous utilisons un segment et une sphere melanges entre eux pour obtenir un avancement de falaise dans 
lequel nous creusons avec l'operateur `Diff`.

### V. Eau
Eau Ligne 58.
Nous avons separe le calcul de l'eau de celui du terrain en utilisant 2 fonctions d'intersection, 
une pour le terrain et une autre pour l'eau, pour pouvoir definir les couleurs differement dans la 
fonction `vec4 Render( in vec3 ro, in vec3 rd, bool pip )` ligne 295.

### VI. Autres
Pour les autres formes et operateurs implementes mais pas forcement testes, il y a :
* operateur Replace : Ligne 88
* Operateur implicit Union : Ligne 100
* Operateur implicit Inter : Ligne 105

  
## Textures
Ca se passe ici `vec4 Render( in vec3 ro, in vec3 rd, bool pip )` ligne 295.

* Soit une couleur pour le ciel : ligne  312
* Si on a de l'eau alors, la couleur qui va avec. 
* Ligne 329-342 : Si la pente est inferieur a un certain degre, que la hauteur est suffisante et l'orientation correcte, on affiche la neige
* sinon, de l'herbe si la hauteur et la pente le permettent.

Pour l'orientation, la hauteur et la pente, on ajoute du bruit pour eviter d'avoir des effets de demarquation trop nets.