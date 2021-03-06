from io import StringIO
import pyjion


def test_single_yield():
    def gen():
        x = 1
        yield x

    g = gen()
    assert next(g) == 1
    assert not pyjion.info(gen).failed


def test_double_yield():
    def gen():
        x = 1
        yield x
        yield 2

    g = gen()
    assert next(g) == 1
    assert next(g) == 2
    assert not pyjion.info(gen).failed


def test_conditional_yield():
    def gen():
        x = 1
        if x == 1:
            yield x
        else:
            yield 2

    g = gen()
    assert next(g) == 1
    assert not pyjion.info(gen).failed


def test_yields_from_iterator():
    def gen():
        yield 1
        yield 2
        yield 3

    g = gen()
    result = list(g)
    assert result == [1, 2, 3]
    assert not pyjion.info(gen).failed


def test_yields_from_range_gen():
    def gen():
        for n in range(10):
            yield f'{n}!'

    result = []
    for x in gen():
        result.append(x)
    assert result == ['0!', '1!', '2!', '3!', '4!', '5!', '6!', '7!', '8!', '9!']
    assert not pyjion.info(gen).failed


def test_yields_from_range_gen_listcomp():
    def gen():
        for n in range(10):
            yield f'{n}!'

    result = [x for x in gen()]
    assert result == ['0!', '1!', '2!', '3!', '4!', '5!', '6!', '7!', '8!', '9!']
    assert not pyjion.info(gen).failed


def test_nested_generator():
    def evens(i):
        for n in range(10):
            if n % 2:
                yield n

    def tens():
        for n in evens(range(100)):
            if n % 10:
                yield f'{n}!'

    assert [x for x in tens()] == ['1!', '3!', '5!', '7!', '9!']
    assert not pyjion.info(tens).failed
    assert not pyjion.info(evens).failed


def test_preservation_of_boxed_variables():
    def cr():
        x = '1'
        yield x
        x = '2'
        yield x
        x = '3'
        yield x
    gen = cr()
    assert (next(gen), next(gen), next(gen)) == ('1', '2', '3')
    assert not pyjion.info(cr).failed


def test_preservation_of_unboxed_variables():
    def cr():
        x = 1
        yield x
        x = 2
        yield x
        x = 3
        yield x
    gen = cr()
    assert (next(gen), next(gen), next(gen)) == (1, 2, 3)
    assert not pyjion.info(cr).failed


def test_range_generator():
    def cr():
        for n in range(10):
            yield f'{n}!'
    assert [x for x in cr()] == ['0!', '1!', '2!', '3!', '4!', '5!', '6!', '7!', '8!', '9!']
    assert not pyjion.info(cr).failed


def test_yield_within_branches():
    def cr():
        x = '2'
        if x == '2':
            yield 'a'
        else:
            yield 'b'
        yield 'c'
        x = x + '2'
        if x == '22':
            yield 'd'
        else:
            yield x
        yield 'c'
    gen = cr()
    assert (next(gen), next(gen), next(gen)) ==  ('a', 'c', 'd')
    assert not pyjion.info(cr).failed


def test_yield_within_branches_for_boxable_vars():
    def cr():
        x = 2
        if x == 2:
            yield 'a'
        else:
            yield 'b'
        yield 'c'
        x = x + 2
        if x == 4:
            yield 'd'
        else:
            yield x
        yield 'c'
    gen = cr()
    assert tuple(gen) == ('a', 'c', 'd', 'c')
    assert not pyjion.info(cr).failed


def test_yield_within_branches_for_boxable_vars_as_iter():
    def cr():
        x = 2
        if x == 2:
            yield 'a'
        else:
            yield 'b'
        yield 'c'
        x = x + 2
        if x == 4:
            yield 'd'
        else:
            yield x
        yield 'c'
    assert [x for x in cr()] == ['a', 'c', 'd', 'c']
    assert not pyjion.info(cr).failed


def test_nested_generator_calling_uncompiled_function():
    def cr1():
        with StringIO("hello") as s:
            for letter in s:
                yield letter

    def cr2():
        for n in "!@#%$^":
            for i in cr1():
                yield n + i

    gen = cr2()

    assert list(gen) == ['!hello', '@hello', '#hello', '%hello', '$hello', '^hello']
    assert pyjion.info(cr1).failed
    assert not pyjion.info(cr2).failed
