from math import exp

class NTC:
    def __init__(self, nominal: float, beta: float) -> None:
        self._nominal = nominal
        self._beta = beta

    def resistance(self, temperature: float) -> float:
        return self._nominal * exp(self._beta * (1 / (temperature + 273.15) - 1 / (25 + 273.15)))

def get_ntc(ntc: NTC, tlow: int, thigh: int):
    results = []
    for t in range(tlow, thigh + 1):
        results.append({'t': t, 'r': ntc.resistance(t)})
    
    # Для бинарного поиска отсортировать по возрастанию
    results = sorted(results, key=lambda x: x['r'])
    return results
