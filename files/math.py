class MyMathClass():
    def __init__(self):
        # do init
        a = 1 + 2
        pass

    # make a strange name of the function
    def IfStatementSum(self, a: int, b):
        if (a < b): 
            return a + b
        else:
            return 0

    # COMMON FUNCTION
    # Крч нужно изменить метод парсинга функций, чтобы проверять, имеется
    #  ли self как первый аргумент или нет. Если есть, только тогда считать, 
    #  что функция - метод класса. Но мне лень
    def commonSum(a, b):
        return a + b

    def multiply(self, a, b, c):
        """my multiply method"""
        a = a * 2
        b = b * 5
        return a * b * c
