# Extended 2048 with Arbitrary Number of Move Directions

An extended implementation of the classic **2048** game, where tile movement  
is performed along an **arbitrary number of shift directions**.  
The field is generated as a regular polygon, and movement occurs along axes  
perpendicular to its edges, fully preserving the core logic of the original game.  
The difference lies not only in the number of directions, but also in the ability  
to flexibly configure the field structure and initial conditions.

---

## ⚙️ Launch Parameters
```json
{
  "w_x": 1920,           // window width
  "w_y": 1080,           // window height
  "axis": 5,             // number of movement directions (>= 2)
  "a": 3,                // edge length (radial field size)
  "isSquare": false,     // corner trimming for square layout when a=2
  "spawn_s_q": 3,        // number of tiles at game start
  "spawn_q": 5,          // number of tiles spawned on each turn
  "spawn_r": 2,          // maximum degree for newly spawned tiles
  "base": 5,             // base value of a tile
  "max_degree": 7        // maximum merge degree
}
```
🔹 **Shift axis** refers to a direction along which tiles can move across the field.  
The `axis` parameter defines the number of such directions and determines the overall field geometry.  
There are no restrictions on parity; the minimum valid value is 2.

---

## 🧮 Mathematical Basis

All tile movement logic is based on classical techniques from linear algebra.  
A key role is played by **basis transformations** used to calculate movement directions  
and convert coordinates relative to new guiding axes.  
Formulas for **coordinate system rotation** and coordinate recalculation are applied  
to ensure consistent movement behavior in any configured geometry.

---

## 🖼️ Examples

- **Classic 2048 (axis = 2)**  
  ![Classic 2048](/docs/classic.png)  
  A field replicating the mechanics of the original 2048 game.  
  Two shift axes are used, corresponding to vertical and horizontal movement.

- **Field with axis = 5 and movement direction visualization**  
  ![Field with shift lines](/docs/move.png)  
  An extended field with five shift axes (ten directions).  
  The image shows movement directions constructed perpendicularly to polygon edges.

---

## 🚦 Status

The project is in working condition — the core game logic is implemented and functions correctly.  
The visual layer and animation are currently unfinished.
