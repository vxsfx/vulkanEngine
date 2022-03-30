class collider:
    def __init__(self, static):
        self.static = static

    def hasCollided(self, other):
        return True


arr = [collider(True), collider(False), collider(True), collider(False), collider(False)]


offset = 0
for col in arr:
    for col2 in arr[offset: ]:
        print("isStatic 1:", col.static)
        print("isStatic 2:", col2.static)
        print(col.hasCollided(col2))
    offset += 1