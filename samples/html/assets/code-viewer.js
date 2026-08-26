document.addEventListener("DOMContentLoaded", () => {
    const contenedorCodigo = document.getElementById("source-code");
    if (!contenedorCodigo) return;

    // Obtener la URL del repositorio público desde un atributo de datos HTML
    const urlArchivoC = contenedorCodigo.getAttribute("data-src");

    fetch(urlArchivoC)
        .then(response => {
            if (!response.ok) throw new Error("No se pudo cargar el archivo de código");
            return response.text();
        })
        .then(codigoTexto => {
            // Asignar el texto plano de forma segura para evitar problemas con < o >
            contenedorCodigo.textContent = codigoTexto;
            
            // Forzar a Prism a resaltar el nuevo bloque inyectado dinámicamente
            Prism.highlightElement(contenedorCodigo);
        })
        .catch(error => {
            contenedorCodigo.textContent = `// Error al cargar el archivo de código fuente: ${error.message}`;
        });
});
