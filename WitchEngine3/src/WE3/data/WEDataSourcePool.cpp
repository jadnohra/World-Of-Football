#include "WEDataSourcePool.h"

#include "../WEAssert.h"

namespace WE {

void DataSourcePool::setTranslator(DataTranslator* pTranslator) {

	mTranslator.set(pTranslator, true);
}

bool DataSourcePool::resolvePossiblyRemoteChunk(DataSourceChunk& chunk, bool shareable, PathResolver* pResolver) {

	if (chunk.isNull()) {

		return false;
	}

	//if (chunk->hasChildren() == false) 
	{

		String attrRemote;

		while (chunk->getAttribute(L"remote", attrRemote)) {

			if (mTranslator.isValid()) {

				BufferString tempStr;

				setRemoteScriptTable(tempStr, chunk.pChunk(), L"remote");
				mTranslator->translate(attrRemote);
			}

			getChunk(attrRemote.c_tstr(), shareable, chunk, chunk.pSource() ? chunk.pSource(): pResolver, true);
			
			if (chunk.isNull()) {

				{
					String absPath;
					PathResolver* resolver = chunk.pSource() ? chunk.pSource(): pResolver;

					if (resolver) {

						resolver->resolvePath(attrRemote.c_tstr(), absPath);
					} 

					log(L"remote chunk [%s] not Found", absPath.c_tstr());

					assrt(false);
				}

				return false;
			}
		}
	}

	return chunk.isValid();
}

void DataSourcePool::setRemoteScriptTable(BufferString& tempStr, DataChunk* pChunk, const TCHAR* remoteAttr) {

	if (pChunk && mTranslator.isValid()) {

		bool inject = false;

		pChunk->scanAttribute(tempStr, L"_inject", inject);

		if (inject) {

			inject = false;
			int attrCount = pChunk->getAttributeCount();

			String key;
			String value;
			
			for (int i = 0; i < attrCount; ++i) {

				if (pChunk->getAttribute(i, &key, &value)) {

					if (inject) {

						assrt(mTranslator->inject(key.c_tstr(), value.c_tstr()) == true);
					}

					if (!inject && key.equals(L"_inject")) {

						value.fromString(inject);

						if (!inject)
							return;
					}
				}
			}
		}
	}
}

DataChunk* DataSourcePool::resolveRemoteChunk(DataChunk& chunk, bool shareable, PathResolver* pResolver, const TCHAR* remoteAttr) {

	HardRef<DataChunk> ret;

	if (chunk.hasChildren()) {

		chunk.AddRef();
		ret = &chunk;

	} else {

		BufferString valRemote;
		BufferString tempStr;
		
		chunk.AddRef();
		ret = &chunk;
		
		while (ret.isValid() && (ret->getAttribute(remoteAttr, valRemote))) {

			SoftRef<DataSource> source = ret->getSource();

			if (mTranslator.isValid()) {

				setRemoteScriptTable(tempStr, ret, remoteAttr);
				mTranslator->translate(valRemote);
			}
			
			ret = getChunk(valRemote.c_tstr(), shareable, source, true, remoteAttr);

			if (ret.ptr() != &chunk) {

				chunk.RemoveRef();
			}

			if (!ret.isValid()) {

				if (source.isValid()) {

					String absPath;

					source->resolvePath(valRemote.c_tstr(), absPath);
					log(L"remote chunk [%s] not Found", absPath.c_tstr());

				} else {

					log(L"remote chunk [%s] not Found", valRemote.c_tstr());
				}

				assrt(false);
			}
		}
	}

	return ret.ptr();
}

DataChunk* DataSourcePool::getRelativeRemoteChunk(DataChunk& chunk, bool shareable, PathResolver* pResolver, const TCHAR* remotePath, const TCHAR* remoteAttr) {

	HardRef<DataChunk> ret;

	BufferString valRemote(remotePath);
		
	ret = &chunk;
	
	if (ret.isValid()) {

		SoftRef<DataSource> src = ret->getSource();

		ret = getChunk(valRemote.c_tstr(), shareable, src, true, remoteAttr);
	}

	if (ret.isValid()) {

		//ref correctness
		SoftRef<DataChunk> temp = ret;

		return resolveRemoteChunk(temp.dref(), shareable, pResolver, remoteAttr);
	}

	return ret.ptr();
}

bool DataSourcePool::resolveRemoteChunk(RefWrap<DataChunk>& chunk, bool shareable, PathResolver* pResolver, const TCHAR* remoteAttr) {

	if (chunk.isNull()) {

		return false;
	}

	chunk.set(resolveRemoteChunk(chunk.dref(), shareable, pResolver, remoteAttr), false);

	return chunk.isValid();
}

bool DataSourcePool::getRelativeRemoteChunk(RefWrap<DataChunk>& chunk, bool shareable, PathResolver* pResolver, const TCHAR* remotePath, const TCHAR* remoteAttr) {

	if (chunk.isNull()) {

		return false;
	}

	chunk.set(getRelativeRemoteChunk(chunk.dref(), shareable, pResolver, remotePath, remoteAttr), false);

	return chunk.isValid();
}

DataChunk* DataSourcePool::getRemoteChild(DataChunk& sourceChunk, bool shareable, const TCHAR* remoteAttr) {

	HardRef<DataChunk> ret;

	BufferString valRemote;

	if ((sourceChunk.getAttribute(remoteAttr, valRemote) == false)) {

		ret = sourceChunk.getFirstChild();

	} else {

		
		BufferString tempStr;
		SoftRef<DataSource> source;

		ret = &sourceChunk;

		while (ret.isValid() && (ret->getAttribute(remoteAttr, valRemote))) {

			source = ret->getSource();

			if (mTranslator.isValid()) {

				setRemoteScriptTable(tempStr, ret, remoteAttr);
				mTranslator->translate(valRemote);
			}

			ret = getChunk(valRemote.c_tstr(), shareable, source.ptr(), true, remoteAttr);
		}

		if (ret.ptr() == &sourceChunk) {

			ret->Release();
			return NULL;
		}
	}

	return ret.ptr();
}

bool DataSourcePool::toRemoteChild(RefWrap<DataChunk>& chunk, bool shareable, const TCHAR* remoteAttr) {

	if (chunk.isNull()) {

		return false;
	}

	chunk.set(getRemoteChild(chunk.dref(), shareable, remoteAttr), false);

	return chunk.isValid();
}

}