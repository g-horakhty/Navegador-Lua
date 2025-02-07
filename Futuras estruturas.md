## 💡 Reflexão sobre o código  

🔍 **Observação:** O código está todo concentrado em um único arquivo.  

✨ **Possível melhoria:** Modularizar a estrutura para melhor organização e manutenção, já pensou travar o programa todo por causa de de uma coisa boba?**

🚀 **Abordagem sugerida:**  
- **Dividir em múltiplos arquivos** para separar responsabilidades.  
- **Criar módulos reutilizáveis** para facilitar futuras implementações.  
- **Organizar diretórios de forma lógica** para um fluxo mais intuitivo.  

📌 **Olha como seria mais ou menos a organização do código** 😎  
├── src/
│   ├── principal.c          # Ponto de entrada do programa
│   ├── gui.c           # Interface gráfica
│   ├── gui.h           # Cabeçalho da interface gráfica
│   ├── webkit.c       # Navegação e Webkit
│   ├── webkit.h       # Cabeçalho da navegação
│   ├── download.c      # Gerenciamento de downloads
│   ├── download.h      # Cabeçalho de downloads
│   ├── history.c       # Histórico de navegação
│   ├── history.h       # Cabeçalho do histórico
├── Makefile            # Script de compilação
