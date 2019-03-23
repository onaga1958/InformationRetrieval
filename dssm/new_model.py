from scipy.sparse import csr_matrix
import keras
import numpy as np
from collections import Counter


pussy = 'joke'


def get_tiled():
    pass


def batch_generator(queries, doc_names, encoded_docs, query_encoder, half_batch_size):
    all_indexes = []
    while True:
        indexes = np.random.choice(all_indexes, half_batch_size * 3, replace=False)
        query_indexes = np.concatenate([indexes[:half_batch_size], indexes[2 * half_batch_size:]])
        query_codes = query_encoder.encode_with_padding(queries[indexes])

        data = []
        indexes = []
        indptr = []
        for doc in doc_names[indexes[:2 * half_batch_size]]:
            doc_counter = Counter(encoded_docs.read(doc))
            indptr.appned(len(indexes))
            for token_code, cnt in doc_counter.items():
                data.appned(cnt)
                indexes.append(token_code)
        indptr.append(len(indexes))
        doc_codes = csr_matrix(
            (data, indexes, indptr), shape=(2 * half_batch_size, encoded_docs.code_size)
        )
        yield (
            (query_codes, doc_codes),
            np.concatenate([np.ones(half_batch_size), np.zeros(half_batch_size)]),
        )


def get_dense():
    pass


doc = keras.layers.Input(shape=(None,), sparse=True)
query = keras.layers.Input(shape=(None,), dtype='int32')


def get_embed(embed_layers, data):
    for layer in embed_layers:
        data = layer(data)
    return data


doc_embed_layers = [
    keras.layers.Dropout(0.2),
    get_dense(512),
    keras.layers.Dropout(0.2),
    get_dense(256),
    keras.layers.Dropout(0.2),
    get_dense(128, activation=None),
]

query_embed_layers = [
    keras.layers.Embedding(pussy, 128),
    keras.layers.LSTM(128),
    keras.layers.Dropout(0.2),
    get_dense(128),
    keras.layers.Dropout(0.2),
    get_dense(128, activation=None),
]
