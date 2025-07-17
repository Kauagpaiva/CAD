import pandas as pd

df = pd.read_csv("resultados/tempos.csv")
media = df.groupby(["versao", "tamanho", "threads"])["tempo"].mean().reset_index()
media.to_csv("resultados/tempos_medios.csv", index=False)
print(media)
