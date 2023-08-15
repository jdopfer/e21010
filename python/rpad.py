import awkward as ak

def rpad(x, y):
    x = ak.nan_to_none(x)
    y = ak.nan_to_none(y)
    max_mul = int(max(max(ak.num(x)), max(ak.num(y))))
    x = ak.pad_none(x, target=max_mul, clip=True)
    y = ak.pad_none(y, target=max_mul, clip=True)
    return ak.to_numpy(ak.ravel(x)), ak.to_numpy(ak.ravel(y))

