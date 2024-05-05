# LoRa_RoutingMesh_Nazaries

## V1
Comunicación punto a punto entre un Nodo y el Gateway.
Establece la tabla de enrutamiento del Nodo. Esta tabla incluye la siguiente información:
- Identificador único (ID) del destino. En este caso siempre será el Gateway -> 0x8C(HEX) ó 140(DEC).
- Próximo salto.
- Número de saltos.
- Secuencia de la ruta en formato cadena de texto. Se le da un formato que el primero es el ID local y el último el ID de destinos, y entre estos los ID de los nodos intermedios por los que debe pasar; todos separados por ";". Por ejemplo: "244;140;".